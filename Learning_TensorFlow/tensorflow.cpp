#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "tensorflow/c/c_api.h"

// Function to read the model
std::vector<char> read_binary(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {
        return buffer;
    } else {
        throw std::runtime_error("Failed to read binary file");
    }
}

int main() {
    // Load the model
    std::string model_path = "mnist_model/saved_model.pb";
    std::vector<char> model_data = read_binary(model_path);

    // Initialize TensorFlow session options
    TF_Status* status = TF_NewStatus();
    TF_SessionOptions* options = TF_NewSessionOptions();
    TF_Buffer* run_options = nullptr;

    // Create a new graph
    TF_Graph* graph = TF_NewGraph();
    TF_Buffer graph_def = {model_data.data(), model_data.size(), nullptr};

    // Import the graph definition
    TF_ImportGraphDefOptions* import_options = TF_NewImportGraphDefOptions();
    TF_GraphImportGraphDef(graph, &graph_def, import_options, status);
    TF_DeleteImportGraphDefOptions(import_options);
    if (TF_GetCode(status) != TF_OK) {
        std::cerr << "Error importing graph: " << TF_Message(status) << std::endl;
        return 1;
    }

    // Create a new session
    TF_Session* session = TF_NewSession(graph, options, status);
    if (TF_GetCode(status) != TF_OK) {
        std::cerr << "Error creating session: " << TF_Message(status) << std::endl;
        return 1;
    }

    // Prepare the input tensor
    // For simplicity, let's assume a single flattened input image of 28x28 pixels
    float image_data[28 * 28] = { /* Fill this with normalized pixel data */ };
    int64_t dims[] = {1, 28 * 28};
    TF_Tensor* input_tensor = TF_NewTensor(TF_FLOAT, dims, 2, image_data, sizeof(image_data), nullptr, nullptr);

    // Prepare the output tensor
    TF_Output output_op = {TF_GraphOperationByName(graph, "output_tensor_name"), 0};
    TF_Tensor* output_tensor = nullptr;

    // Run the session
    TF_SessionRun(session,
                  run_options,
                  nullptr, &input_tensor, 1, // Input
                  &output_op, &output_tensor, 1, // Output
                  nullptr, 0, // No targets
                  nullptr, // Run metadata
                  status);

    if (TF_GetCode(status) != TF_OK) {
        std::cerr << "Error running the session: " << TF_Message(status) << std::endl;
        return 1;
    }

    // Process the output
    float* output_data = static_cast<float*>(TF_TensorData(output_tensor));
    std::cout << "Predicted digit: " << std::distance(output_data, std::max_element(output_data, output_data + 10)) << std::endl;

    // Clean up
    TF_DeleteTensor(input_tensor);
    TF_DeleteTensor(output_tensor);
    TF_CloseSession(session, status);
    TF_DeleteSession(session, status);
    TF_DeleteGraph(graph);
    TF_DeleteSessionOptions(options);
    TF_DeleteStatus(status);

    std::cout << "TensorFlow model executed successfully." << std::endl;
    return 0;
}
