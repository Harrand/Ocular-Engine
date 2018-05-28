#ifndef TOPAZ_WINDOW_HPP
#define TOPAZ_WINDOW_HPP
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <string>
#include "listener.hpp"
#include "gui.hpp"

namespace tz
{
    /// Window-centre flag. Use this to centre a Window.
    constexpr auto centred_window = SDL_WINDOWPOS_CENTERED;
    namespace gui
    {
        enum class MessageBoxType : Uint32{ERROR = SDL_MESSAGEBOX_ERROR, WARNING = SDL_MESSAGEBOX_WARNING, INFO = SDL_MESSAGEBOX_INFORMATION};
    }
}

class Listener;

class Window
{
public:
    friend class MessageBox;
    /**
     * Constructs a Window from all parameters.
     * @param title - Title of the Window bar
     * @param x_pixels - Number of pixels from the left of the screen to create the window
     * @param y_pixels - Number of pixels from the top of the screen to create the window
     * @param width_pixels - Width of the window, in pixels
     * @param height_pixels - Height of the window, in pixels
     */
    Window(std::string title, int x_pixels, int y_pixels, int width_pixels, int height_pixels);
    /**
     * Constructs a Window from compact parameters.
     * @param title - Title of the Window bar
     * @param position_pixel_space - 2-dimensional Vector representing the position of the top-left of the Window on the screen, in pixels
     * @param dimensions_pixel_space - 2-dimensional Vector representing the corresponding width and height of the Windo on the screen, in pixels
     */
    Window(std::string title, const Vector2<int>& position_pixel_space, const Vector2<int>& dimensions_pixel_space);
    /**
     * Safely dispose of all SDL components and allow any subclasses to also have this behaviour.
     */
    virtual ~Window();
    /**
	 * Update (and render) the window.
	 */
    void update();
    /**
     * Get the position of the top-left of the Window, depending on what screen-space is specified.
     * @param screen_space - The specified screen-space to format the return value as
     * @return - A Vector2I if ScreenSpace::PIXELS is passed, or a Vector2F if ScreenSpace::NORMALISED is passed
     */
    std::variant<Vector2<int>, Vector2F> get_position(tz::gui::ScreenSpace screen_space) const;
    /**
     * Get the position of the top-left of the Window, in pixels.
     * @return - 2-dimensional Vector corresponding to the x and y coordinates of the top-left of the Window
     */
    Vector2<int> get_position_pixels() const;
    /**
     * Set the position of the top-left of the Window, depending on what screen-space is specified.
     * @param position - Either a Vector2I representing the pixel-coordinate (should be done if ScreenSpace::PIXELS is passed), or a Vector2F representing the normalised-screen-space coordinate (should be done if ScreenSpace::NORMALISED is passed)
     * @param screen_space - The specified screen-space to interpret the input as
     */
    void set_position(std::variant<Vector2<int>, Vector2F> position, tz::gui::ScreenSpace screen_space);
    /**
     * Set the position of the top-left of the Window, in pixel-screen-space.
     * @param position_pixels - 2-dimensional Vector representing the position of the top-left of the Window, in pixels
     */
    void set_position_pixels(Vector2<int> position_pixels);
    /**
     * Set the position of the Window such that it sits directly in the middle of the display.
     * @param mask - Mask of which dimensions to apply the centering on (e.g, passing {true, false} performs centering only on the horizontal-axis)
     */
    void centre_position(const Vector2<bool>& mask = {true, true});
    /**
     * Get the dimensions of the Window, depending on what screen-space is specified.
     * @param screen_space - The specified screen-space to format the return value as
     * @return - A Vector2I if ScreenSpace::PIXELS is passed, or a Vector2F if ScreenSpace::NORMALISED is passed
     */
    std::variant<Vector2<int>, Vector2F> get_dimensions(tz::gui::ScreenSpace screen_space) const;
    /**
     * Get the dimensions of the Window, in pixels.
     * @return - 2-dimensional Vector corresponding to the x and y coordinates of the top-left of the Window
     */
    Vector2<int> get_dimensions_pixels() const;
    /**
     * Get the width of the Window, in pixels.
     * @return - Number of pixels wide for this Window
     */
    int get_width() const;
    /**
     * Get the height of the Window, in pixels.
     * @return - Number of pixels high for this Window
     */
    int get_height() const;
    /**
     * Get whether the Window has been requested to close.
     * @return - True if the Window should be closed. False otherwise
     */
    bool is_close_requested() const;

    /**
     * Specifies which techniques a Window will use each frame to update its buffers.
     */
    enum class SwapIntervalType : int
    {
        LATE_SWAP_TEARING = -1,
        IMMEDIATE_UPDATES = 0,
        VSYNC = 1,
    };

    /**
	 * Query which SwapIntervalType is being used for this Window.
	 * @return - The SwapIntervalType this Window is using
	 */
    SwapIntervalType get_swap_interval_type() const;
    /**
     * Specify which SwapIntervalType should be used for this Window.
     * @param type - The SwapIntervalType the Window should use
     */
    void set_swap_interval_type(SwapIntervalType type) const;
    /**
     * Get the title of this Window.
     * @return - Title of the Window
     */
    std::string get_title() const;
    /**
	 * Change the string shown on the Window title.
	 * @param new_title - The new title of the Window.
	 */
    void set_title(const std::string& new_title);
    /**
     * Represents which type of Fullscreen states are available for a Window.
     */
    enum class FullscreenType : Uint32
    {
        VIDEO_MODE = SDL_WINDOW_FULLSCREEN,
        DESKTOP_MODE = SDL_WINDOW_FULLSCREEN_DESKTOP,
        WINDOWED_MODE = 0,
    };
    /**
     * Query whether the Window is in either of the two fullscreen-types.
     * @return - True if the Window is in either of the two fullscreen-types. False otherwise
     */
    bool is_fullscreen() const;
    /**
     * Get the fullscreen-type that the Window is currently in.
     * @return - Either VIDEO_MODE (true fullscreen), DESKTOP_MODE (fake fullscreen) or WINDOWED_MODE (not fullscreen).
     */
    FullscreenType get_fullscreen() const;
    /**
     * Specify the fullscreen-type that the Window should be in.
     * @param type - FullscreenType the Window should be in.
     */
    void set_fullscreen(FullscreenType type) const;
    /**
     * Query whether the window is currently minimised.
     * @return - True if Window is minimised. Otherwise false
     */
    bool is_minimised() const;
    /**
     * Set whether the window is minimised or not.
     * @param minimised - Whether the Window should be minimised or not
     */
    void set_minimised(bool minimised);
    /**
     * Query whether the Window has a border or not.
     * @return - True if the Window has a border. False otherwise
     */
    bool has_border() const;
    /**
     * Set whether the Window has a border or not.
     * @param has_border - Whether the Window should have a border
     */
    void set_has_border(bool has_border);
    bool focused() const;
    void focus() const;
    bool mouse_inside() const;
    bool mouse_trapped() const;
    /**
     * Ensure that all OpenGL render calls in the future render to this Window's framebuffer.
     */
    void set_render_target() const;
    /**
	 * Clear the Window's framebuffer and set the background to a desired colour.
	 * @param mask - Which buffer-bits to clear
	 * @param r - Redness of the background colour, between 0.0 and 1.0
	 * @param g - Greenness of the background colour, between 0.0 and 1.0
	 * @param b - Blueness of the background colour, between 0.0 and 1.0
	 * @param a - Alpha intensity of the background colour, between 0.0 and 1.0
	 */
    void clear(GLbitfield mask = (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT), float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) const;
    /**
     * Register an existing listener to this Window.
     * @param l - The polymorphic listener that this Window should administrate
     */
    void register_listener(Listener& l);
    /**
     * Deregister a listener that is currently being administrated by this Window.
     * @param l - The listener which should be de-registered
     */
    void deregister_listener(Listener& l);
    std::unordered_set<GUI*> get_children() const;
    template<class GUIType, typename... Args>
    GUIType* emplace_child(Args&&... args);
    bool add_child(GUI* gui);

private:
    /// Container of all registered polymorphic listeners.
    std::unordered_map<unsigned int, Listener*> registered_listeners;
    std::string title;
    Vector2<int> position_pixel_space, dimensions_pixel_space;
    SDL_Window* sdl_window;
    SDL_GLContext sdl_gl_context;
    bool close_requested;
    std::unordered_set<GUI*> children;
    std::unordered_set<std::shared_ptr<GUI>> heap_children;
};

class MessageBox
{
public:
    MessageBox(tz::gui::MessageBoxType type, std::string title, std::string message, Window* parent);
    bool display() const;
private:
    tz::gui::MessageBoxType type;
    std::string title, message;
    Window* parent;
};

#include "window.inl"

#endif //TOPAZ_WINDOW_HPP
