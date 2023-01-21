#include "RequestUtils.hpp"

response RequestUtils::manage(const uint8_t *incomingData, int len,
                              request_handler_t request_handler, send_op_handler_t send_op_handler, subscribe_op_handler_t subscribe_op_handler, ping_op_handler_t ping_op_handler) {
    request deserializedRequest = request_init_default;
    response response = response_init_zero;
    bool deserialized = deserialize(&deserializedRequest, incomingData, len);

    if (deserialized) {
        printMacAndLenPacket(deserializedRequest.client_mac, len, "Packet received from: ");
        response = manage(&deserializedRequest, send_op_handler, subscribe_op_handler, ping_op_handler);
        request_handler(&deserializedRequest, incomingData, len, &response);
    } else {
        debugln("Error deserializing request.");
    }
    return response;
}

response RequestUtils::manage(request *request,
                              send_op_handler_t send_op_handler, subscribe_op_handler_t subscribe_op_handler, ping_op_handler_t ping_op_handler) {
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
            case request_Operation_qRequest_tag:
                printSubscribeOperation(&request->operations[count].op.qRequest);
                subscribe_op_handler(request, &request->operations[count].op.qRequest, opResponse);
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

request RequestUtils::createRequest(const char *clientName, uint8_t macWhereToSendTheResponse[6], int32_t messageType) {
    request request = request_init_zero;
    strcpy(request.client_id, clientName);
    request.operations_count = 0;
    memcpy(request.client_mac, macWhereToSendTheResponse, 6);
    request.message_type = messageType;
    return request;
}

void RequestUtils::buildSendOperation(request *request, const char *payload, const char *queue, bool retain) {
    request_Operation operationSend = request_Operation_init_zero;
    operationSend.which_op = request_Operation_send_tag;
    strcpy(operationSend.op.send.payload, payload);
    strcpy(operationSend.op.send.queue, queue);
    operationSend.op.send.persist = retain;
    addRequestOperationToRequest(request, operationSend);
}

void RequestUtils::buildSubscribeOperation(request *request, const char *queue, bool clear) {
    request_Operation operationSubscribe = request_Operation_init_zero;
    operationSubscribe.which_op = request_Operation_qRequest_tag;
    strcpy(operationSubscribe.op.qRequest.queue, queue);
    operationSubscribe.op.qRequest.clear = clear;
    addRequestOperationToRequest(request, operationSubscribe);
}

void RequestUtils::buildPingOperation(request *request, int32_t ping) {
    request_Operation operationPing = request_Operation_init_zero;
    operationPing.which_op = request_Operation_ping_tag;
    operationPing.op.ping.num = ping;
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
#ifdef PRINT_REQUEST_RESPONSE_FLAG
    debug("Client: ");
    debugln(request->client_id);
    debug("Operations: ");
    debugln(request->operations_count);
    debug("MessageType: ");
    debugln(request->message_type);
#endif
}

void RequestUtils::printSendOperation(request_Send *send) {
#ifdef PRINT_REQUEST_RESPONSE_FLAG
    debugln("> Send operation");
    debug(">>> Queue: ");
    debugln(send->queue);
    debug(">>> Payload: ");
    debugln(send->payload);
    debug(">>> Persist: ");
    debugln(send->persist);
#endif
}

void RequestUtils::printSubscribeOperation(request_Subscribe *subscribe) {
#ifdef PRINT_REQUEST_RESPONSE_FLAG
    debugln("> Subscribe operation");
    debug(">>> Queue: ");
    debugln(subscribe->queue);
    debug(">>> Clear: ");
    debugln(subscribe->clear);
#endif
}

void RequestUtils::printPingOperation(request_Ping *ping) {
#ifdef PRINT_REQUEST_RESPONSE_FLAG
    debugln("> Ping operation");
    debug(">>> Num: ");
    debugln(ping->num);
#endif
}

void RequestUtils::printUnknownOperation(pb_size_t which_op) {
#ifdef PRINT_REQUEST_RESPONSE_FLAG
    debug("> Unknown: ");
    debugln(which_op);
#endif
}

void requestHandlerDummy(request *deserializedRequest, const uint8_t *serializedRequest, int len, response *response) {}
void sendOpHandlerDummy(request *request, request_Send *sendOp, response_OpResponse *opResponse) {}
void subscribeOpHandlerDummy(request *request, request_Subscribe *subscribeOp, response_OpResponse *opResponse) {}
void pingOpHandlerDummy(request *request, request_Ping *pingOp, response_OpResponse *opResponse) {}