#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>

class HelloTriangleApplication {
private:
    GLFWwindow* window;
    const uint32_t INITIAL_WINDOW_WIDTH = 800;
    const uint32_t INITIAL_WINDOW_HEIGHT = 600;

    VkInstance instance;

    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif

public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    void initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window = glfwCreateWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "Vulkan Test", nullptr, nullptr);
    }

    void initVulkan() {
        createInstance();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }

// Init vulkan methods
private:
    void createInstance() {
        // Check that you can find validation layerss
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }
        // Create app info | Description of the app, will be part of createInfo
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Vulkan Test";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        // Create info | global definitions
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        // Setup GLFW's required vulkan extensions
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        // Get GLFW's required extensions
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        // Enable those extensions
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;

        // TODO: figure this out
        createInfo.enabledLayerCount = 0;
        
        // Check if it all went well
        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }

        // Setup holder for the the available extensions
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        // Actually get the available extensions and store them
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        // Print out the available extensions
        std::cout << "available extensions:\n";
        for (const auto& extension : extensions) {
            std::cout << '\t' << extension.extensionName << std::endl;
        }

        // Challenge function to learn, not required
        checkIfGLFWExtensionsAreSupported(extensions, glfwExtensions, glfwExtensionCount);

        // Enable validation layers
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }
    }

    void checkIfGLFWExtensionsAreSupported(const std::vector<VkExtensionProperties> supportedExtensions, const char** glfwExtensions, const uint32_t glfwExtensionsCount) {
        bool isNotSupported = false;
        for (uint32_t i = 0; i < glfwExtensionsCount; i++) {
            bool foundSupported = false;
            for (const auto& extension : supportedExtensions) {
                if (std::string(glfwExtensions[i]) == extension.extensionName) {
                    foundSupported = true;
                    break;
                }
            }
            if (!foundSupported) {
                isNotSupported = true;
                break;
            }
        }
        if (isNotSupported) {
            std::cout << "OMG GLFW DOESN'T HAVE THE RIGHT EXTENSIONS" << std::endl;
        } else {
            std::cout << "GLFW has the right supported extensions I think" << std::endl;
        }
    }

    bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}