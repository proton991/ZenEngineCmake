#include <utils/timer.hpp>
#include <logging.hpp>
#include <vk_helper/context.hpp>

using namespace zen;
int main() {
  logger::info("ZEN_SHADER_PATH: {}", ZEN_SHADER_PATH);
	util::FrameTimer timer;
	timer.TimeStep();
  vkh::Context vk_ctx;
}