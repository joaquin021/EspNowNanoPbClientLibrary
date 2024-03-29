#include "ResponseUtils.hpp"

void ResponseUtils::manage(const uint8_t *incomingData, int len, response_handler_t response_handler, op_responser_handler_t op_responser_handler) {
    response deserializedResponse = response_init_zero;
    bool deserialized = deserialize(&deserializedResponse, incomingData, len);
    if (deserialized) {
        printFromToMacAndLenPacket(deserializedResponse.from_mac, deserializedResponse.to_mac, len);
        manage(&deserializedResponse, op_responser_handler);
        response_handler(&deserializedResponse, incomingData, len);
    } else {
        logErrorln("ResponseUtils\t\tError deserializing response.");
    }
}

void ResponseUtils::manage(response *response, op_responser_handler_t op_responser_handler) {
    printResponseData(response);
    for (int count = 0; count < response->opResponses_count; count++) {
        response_OpResponse opResponse = response->opResponses[count];
        printOpResponse(&opResponse, count);
        op_responser_handler(response, &opResponse, count);
    }
}

response ResponseUtils::createResponse(request *request) {
    response response = response_init_zero;
    response.opResponses_count = request->operations_count;
    memcpy(response.from_mac, request->to_mac, sizeof(request->to_mac));
    memcpy(response.to_mac, request->from_mac, sizeof(request->from_mac));
    response.message_type = request->message_type;
    return response;
}

void ResponseUtils::buildOpResponse(response_OpResponse *opResponse, response_Result result_code, const char *payload) {
    opResponse->result_code = result_code;
    if (payload) {
        strlcpy(opResponse->payload, payload, sizeof(opResponse->payload));
    }
}

int ResponseUtils::serialize(uint8_t *buffer, response *response) {
    pb_ostream_t myStream = pb_ostream_from_buffer(buffer, ESPNOW_BUFFERSIZE);
    pb_encode(&myStream, response_fields, response);
    return myStream.bytes_written;
}

bool ResponseUtils::deserialize(response *deserializedResponse, const uint8_t *incomingData, int len) {
    pb_istream_t iStream = pb_istream_from_buffer(incomingData, len);
    return pb_decode(&iStream, response_fields, deserializedResponse);
}

void ResponseUtils::printResponseData(response *deserializedResponse) {
    logTrace("ResponseUtils\t\tOperations: ");
    logTraceln(deserializedResponse->opResponses_count);
    logTrace("ResponseUtils\t\tMessageType: ");
    logTraceln(deserializedResponse->message_type);
}

void ResponseUtils::printOpResponse(response_OpResponse *opResponse, int operationNumber) {
    logTraceln("ResponseUtils\t\t> Operation " + String(operationNumber));
    logTracef("ResponseUtils\t\t>>> OperationType: %d\n", opResponse->operation_type);
    logTrace("ResponseUtils\t\t>>> Result Code: ");
    logTraceln(opResponse->result_code);
    logTrace("ResponseUtils\t\t>>> Payload: ");
    logTraceln(opResponse->payload);
}

void responseHandlerDummy(response *deserializedResponse, const uint8_t *serializedResponse, int len) {}
void opResponseHandlerDummy(response *response, response_OpResponse *opResponse, int operationNumber) {}