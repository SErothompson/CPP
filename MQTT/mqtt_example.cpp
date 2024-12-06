#include <iostream>
#include "mqtt/client.h"

const std::string ADDRESS{"tcp://localhost:1883"};
const std::string CLIENT_ID{"example_client_id"};
const std::string TOPIC{"test/topic"};
const std::string PAYLOAD{"Hello EMQX!"};

const int QOS = 1;
const int TIMEOUT = 10000;

int main() {
    mqtt::client client(ADDRESS, CLIENT_ID);

    mqtt::connect_options connOpts;
    connOpts.set_clean_session(true);

    try {
        std::cout << "Connecting to the MQTT broker..." << std::endl;
        client.connect(connOpts);
        std::cout << "Connected." << std::endl;

        mqtt::message_ptr pubmsg = mqtt::make_message(TOPIC, PAYLOAD);
        pubmsg->set_qos(QOS);
        client.publish(pubmsg);
        std::cout << "Message published." << std::endl;

        client.subscribe(TOPIC, QOS);

        while (true) {
            auto msg = client.consume_message();
            if (msg) {
                std::cout << "Received message: '" << msg->to_string() << "' on topic: '" << msg->get_topic() << "'" << std::endl;
            }
        }

        client.disconnect();
        std::cout << "Disconnected." << std::endl;
    } catch (const mqtt::exception &exc) {
        std::cerr << exc.what() << std::endl;
        return 1;
    }

    return 0;
}
