#include <mosquitto.h>
#include <iostream>
#include <cstring>

void on_connect(struct mosquitto *mosq, void *obj, int rc) {
    std::cout << "Connected with code " << rc << std::endl;
    if (rc == 0) {
        mosquitto_subscribe(mosq, NULL, "test/topic", 0);
    }
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
    std::cout << "Received message: " << (char *)msg->payload << " on topic " << msg->topic << std::endl;
}

int main() {
    mosquitto_lib_init();

    struct mosquitto *mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        std::cerr << "Failed to create mosquitto instance" << std::endl;
        return 1;
    }

    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);

    if (mosquitto_connect(mosq, "localhost", 1883, 60) != MOSQ_ERR_SUCCESS) {
        std::cerr << "Failed to connect to broker" << std::endl;
        return 1;
    }

    mosquitto_loop_start(mosq);

    std::string input;
    while (true) {
        std::getline(std::cin, input);
        if (input == "quit") break;
        mosquitto_publish(mosq, NULL, "test/topic", input.size(), input.c_str(), 0, false);
    }

    mosquitto_loop_stop(mosq, true);
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}
