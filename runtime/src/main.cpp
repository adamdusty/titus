#include <cstdint>
#include <titus/sdk.hpp>

#include <flecs.h>
#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/LogMacros.h>
#include <quill/sinks/ConsoleSink.h>

struct application_state {
    enum class status {
        initialized,
        running,
        paused,
        waiting_for_deinitialization,
    } status = application_state::status::initialized;
};

auto main() -> int {
    // Initialization and configuration of application
    auto state = application_state{};
    quill::Backend::start();
    quill::Logger* logger = quill::Frontend::create_or_get_logger(
        "root", quill::Frontend::create_or_get_sink<quill::ConsoleSink>("sink_id_1"));

    LOG_INFO(logger, "Logger initialized");

    // Create flecs world
    auto world = flecs::world{};

    // Load plugin from list of plugin paths

    // Execute plugin init functions

    // Begin running systems
    std::uint64_t count = 0;
    state.status        = application_state::status::running;
    while(state.status == application_state::status::running) {
        world.progress();
        LOG_INFO(logger, "Iteration: {}", count++);

        if(count > 10) {
            state.status = application_state::status::waiting_for_deinitialization;
        }
    }

    // Execute plugin deinit functions

    // Clean up application

    return 0;
}