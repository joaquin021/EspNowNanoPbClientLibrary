#include "RequestUtils.hpp"

response RequestUtils::manage(const uint8_t *incomingData, int len,
                              request_handler_t request_handler, send_op_handler_t send_op_handler,
                              subscribe_op_handler_t subscribe_op_handler, unsubscribe_op_handler_t unsubscribe_op_handler,
                              ping_op_handler_t ping_op_handler) {
    request deserializedRequest = request_init_default;
    response response = response_init_zero;
    bool deserialized = deserialize(&deserializedRequest, incomingData, len);

    if (deserialized) {
        printFromToMacAndLenPacket(deserializedRequest.from_mac, deserializedRequest.to_mac, len);
        response = manage(&deserializedRequest, send_op_handler, subscribe_op_handler, unsubscribe_op_handler, ping_op_handler);
        request_handler(&deserializedRequest, incomingData, len, &response);
    } else {
        logErrorln("RequestUtils\t\tError deserializing request.");
    }
    return response;
}

response RequestUtils::manage(request *request, send_op_handler_t send_op_handler, subscribe_op_handler_t subscribe_op_handler,
                              unsubscribe_op_handler_t unsubscribe_op_handler, ping_op_handler_t ping_op_handler) {
    printRequestData(request);
    response response = ResponseUtils::getInstance().createResponse(request);
    for (int count = 0; count < request->operations_count; count++) {
        pb_size_t which_op = request->operations[count].which_op;
        response_OpResponse *opResponse = &response.opResponses[count];

        switch (which_op) {
            case request_Operation_send_tag:
                printSendOperation(&request->operations[count].op.send);
                send_op_handler(request, &request->operations[count].op.send, opResponse);
                break;
            case request_Operation_subscribe_tag:
                printSubscribeOperation(&request->operations[count].op.subscribe);
                subscribe_op_handler(request, &request->operations[count].op.subscribe, opResponse);
                break;
            case request_Operation_unsubscribe_tag:
                printUnSubscribeOperation(&request->operations[count].op.unsubscribe);
                unsubscribe_op_handler(request, &request->operations[count].op.unsubscribe, opResponse);
                break;
            case request_Operation_ping_tag:
                printPingOperation(&request->operations[count].op.ping);
                ping_op_handler(request, &request->operations[count].op.ping, opResponse);
                break;
            default:
                printUnknownOperation(which_op);
                break;
        }
    }
    return response;
}

request RequestUtils::createRequest(const char *clientName, uint8_t fromMac[6], uint8_t toMac[6], int32_t messageType) {
    request request = request_init_zero;
    strcpy(request.client_id, clientName);
    request.operations_count = 0;
    memcpy(request.from_mac, fromMac, 6);
    memcpy(request.to_mac, toMac, 6);
    request.message_type = messageType;
    return request;
}

void RequestUtils::buildSendOperation(request *request, const char *payload, const char *queue, bool retain, int32_t operationType) {
    request_Operation operationSend = request_Operation_init_zero;
    operationSend.which_op = request_Operation_send_tag;
    strcpy(operationSend.op.send.payload, payload);
    strcpy(operationSend.op.send.queue, queue);
    operationSend.op.send.persist = retain;
    operationSend.op.send.operation_type = operationType;
    addRequestOperationToRequest(request, operationSend);
}

void RequestUtils::buildSubscribeOperation(request *request, const char *queue, int32_t operationType) {
    request_Operation operationSubscribe = request_Operation_init_zero;
    operationSubscribe.which_op = request_Operation_subscribe_tag;
    strcpy(operationSubscribe.op.subscribe.queue, queue);
    operationSubscribe.op.subscribe.operation_type = operationType;
    addRequestOperationToRequest(request, operationSubscribe);
}

void RequestUtils::buildPingOperation(request *request, int32_t ping, int32_t operationType) {
    request_Operation operationPing = request_Operation_init_zero;
    operationPing.which_op = request_Operation_ping_tag;
    operationPing.op.ping.num = ping;
    operationPing.op.ping.operation_type = operationType;
    addRequestOperationToRequest(request, operationPing);
}

void RequestUtils::addRequestOperationToRequest(request *request, request_Operation requestOperation) {
    int nextOperationsCount = request->operations_count + 1;
    request->operations[nextOperationsCount - 1] = requestOperation;
    request->operations_count = nextOperationsCount;
}

int RequestUtils::serialize(uint8_t *buffer, request *request) {
    pb_ostream_t myStream = pb_ostream_from_buffer(buffer, ESPNOW_BUFFERSIZE);
    pb_encode(&myStream, request_fields, request);
    return myStream.bytes_written;
}

bool RequestUtils::deserialize(request *deserializedRequest, const uint8_t *incomingData, int len) {
    pb_istream_t iStream = pb_istream_from_buffer(incomingData, len);
    return pb_decode(&iStream, request_fields, deserializedRequest);
}

void RequestUtils::printRequestData(request *request) {
    logTrace("RequestUtils\t\tClient: ");
    logTraceln(request->client_id);
    logTrace("RequestUtils\t\tOperations: ");
    logTraceln(request->operations_count);
    logTrace("RequestUtils\t\tMessageType: ");
    logTraceln(request->message_type);
}

void RequestUtils::printSendOperation(request_Send *send) {
    logTraceln("RequestUtils\t\t> Send operation");
    logTrace("RequestUtils\t\t>>> OperationType: ");
    logTraceln(send->operation_type);
    logTrace("RequestUtils\t\t>>> Queue: ");
    logTraceln(send->queue);
    logTrace("RequestUtils\t\t>>> Payload: ");
    logTraceln(send->payload);
    logTrace("RequestUtils\t\t>>> Persist: ");
    logTraceln(send->persist);
}

void RequestUtils::printSubscribeOperation(request_Subscribe *subscribe) {
    logTraceln("RequestUtils\t\t> Subscribe operation");
    logTrace("RequestUtils\t\t>>> OperationType: ");
    logTraceln(subscribe->operation_type);
    logTrace("RequestUtils\t\t>>> Queue: ");
    logTraceln(subscribe->queue);
}

void RequestUtils::printUnSubscribeOperation(request_Unsubscribe *unSubscribe) {
    logTraceln("RequestUtils\t\t> Uubscribe operation");
    logTrace("RequestUtils\t\t>>> OperationType: ");
    logTraceln(unSubscribe->operation_type);
    logTrace("RequestUtils\t\t>>> Queue: ");
    logTraceln(unSubscribe->queue);
}

void RequestUtils::printPingOperation(request_Ping *ping) {
    logTraceln("RequestUtils\t\t> Ping operation");
    logTrace("RequestUtils\t\t>>> OperationType: ");
    logTraceln(ping->operation_type);
    logTrace("RequestUtils\t\t>>> Num: ");
    logTraceln(ping->num);
}

void RequestUtils::printUnknownOperation(pb_size_t which_op) {
    logTrace("RequestUtils\t\t> Unknown: ");
    logTraceln(which_op);
}

void requestHandlerDummy(request *deserializedRequest, const uint8_t *serializedRequest, int len, response *response) {}
void sendOpHandlerDummy(request *request, request_Send *sendOp, response_OpResponse *opResponse) {}
void subscribeOpHandlerDummy(request *request, request_Subscribe *subscribeOp, response_OpResponse *opResponse) {}
void unSubscribeOpHandlerDummy(request *request, request_Unsubscribe *unSubscribeOp, response_OpResponse *opResponse) {}
void pingOpHandlerDummy(request *request, request_Ping *pingOp, response_OpResponse *opResponse) {}