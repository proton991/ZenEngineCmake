#ifndef ZENENGINE_WINDOW_SYSTEM_HPP
#define ZENENGINE_WINDOW_SYSTEM_HPP

struct GLFWwindow;
namespace zen::sys {
struct Extend2D {
  int width;
  int height;
};

struct WindowConfig {
  unsigned int width;
  unsigned int height;
  bool resizable;
  const char* title;
};

class Window {
public:
  static WindowConfig default_config();
  explicit Window(const WindowConfig& config);
  Window(Window&&)                 = delete;
  Window& operator=(Window&&)      = delete;
  Window(const Window&)            = delete;
  Window& operator=(const Window&) = delete;
  [[nodiscard]] GLFWwindow* handle() const { return m_window; }

  void set_glfw_callbacks();

  void wait_for_focus();

  void swap_buffers() const;

  void enable_cursor() const;

  void disable_cursor() const;

  [[nodiscard]] Extend2D get_framebuffer_size() const;

  void update();

  [[nodiscard]] auto should_close() const { return m_data.should_close; }
  [[nodiscard]] auto should_resize() const { return m_data.should_resize; }

  void resize() { m_data.should_resize = false; }

  [[nodiscard]] auto get_width() const { return m_data.width; }
  [[nodiscard]] auto get_height() const { return m_data.height; }
  [[nodiscard]] auto get_aspect() const { return float(m_data.width) / float(m_data.height); }

private:
  bool center_window();
  void destroy();

  GLFWwindow* m_window{nullptr};

  struct WindowData {
    // size
    int width;
    int height;

    // status
    bool should_close{false};
    bool show_cursor{true};
    bool should_resize{false};
  } m_data;
};
}  // namespace zen::sys
#endif  //EASYGRAPHICS_WINDOW_SYSTEM_HPP
