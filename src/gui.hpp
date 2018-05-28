#ifndef TOPAZ_GUI_HPP
#define TOPAZ_GUI_HPP
#include <SDL2/SDL.h>
#include <unordered_set>
#include <memory>
#include <variant>
#include "graphics/mesh.hpp"
#include "graphics/shader.hpp"
#include "camera.hpp"

namespace tz::gui
{
        /**
         * Consider a 800x600 pixel screen:
         * for ScreenSpace::PIXELS:
         * +------------+
         * |            |   ^
         * |            |  600
         * |            |   v
         * +------------+
         *     < 800 >
         *
         * for ScreenSpace::NORMALISED:
         * +------------+
         * |            |   ^
         * |            |   1
         * |            |   v
         * +------------+
         *     < 1 >
         */
    enum class ScreenSpace : unsigned int { PIXELS, NORMALISED };
}

class GUI
{
public:
    friend class Window;
    GUI(Vector2<int> position_local_pixel_space, Vector2<int> dimensions_local_pixel_space, GUI* parent = nullptr, std::initializer_list<GUI*> children = {});
    virtual void render(Shader& shader, int window_width_pixels, int window_height_pixels) const;
    /**
     * Get the x-coordinate of the top-left of this GUI element on the screen.
     * @return - X-coordinate of the GUI on the screen
     */
    int get_x() const;
    /**
     * Get the y-coordinate of the top-left of this GUI element on the screen.
     * @return - Y-coordinate of the GUI on the screen
     */
    int get_y() const;
    /**
     * Get the width of this GUI, in pixels.
     * @return - Width of GUI in pixels.
     */
    int get_width() const;
    /**
     * Get the height of this GUI, in pixels.
     * @return - Height of GUI in pixels.
     */
    int get_height() const;
    /**
     * Get proportion of the screen that the width of this GUI takes up.
     * @return - Float value between 0.0 and 1.0f
     */
    float get_normalised_screen_width() const;
    /**
     * Get proportion of the screen that the height of this GUI takes up.
     * @return - Float value between 0.0 and 1.0f
     */
    float get_normalised_screen_height() const;
    GUI* get_parent() const;
    void set_parent(GUI* new_parent);
    std::unordered_set<GUI*> get_children() const;
    template<class GUIType, typename... Args>
    GUIType* emplace_child(Args&&... args);
    bool add_child(GUI* gui);
protected:
    /**
     * Get the local-position (relative to the parent, or screen if there isn't one) of the top-left of the GUI, depending on what screen-space is specified.
     * @param screen_space - The specified screen-space to format the return value as
     * @return - A Vector2I if ScreenSpace::PIXELS is passed, or a Vector2F if ScreenSpace::NORMALISED is passed
     */
    std::variant<Vector2<int>, Vector2F> get_local_position(tz::gui::ScreenSpace screen_space) const;
    Vector2<int> get_local_position_pixel_space() const;
    Vector2F get_local_position_normalised_space() const;
    std::variant<Vector2<int>, Vector2F> get_screen_position(tz::gui::ScreenSpace screen_space) const;
    Vector2<int> get_screen_position_pixel_space() const;
    Vector2F get_screen_position_normalised_space() const;

    Vector2<int> position_local_pixel_space, dimensions_local_pixel_space;
    GUI* parent;
    std::unordered_set<GUI*> children;
    std::unordered_set<std::shared_ptr<GUI>> heap_children;
    Mesh mesh;
};

/**
 * Common gui helper functions, such as space-conversions.
 */
namespace tz::util::gui
{
    /**
     * Utility functions specific to the screen display.
     */
    namespace display
    {
        /**
         * Get the resolution of the current display.
         * @return - {w, h} in pixels
         */
        Vector2<int> resolution();
        /**
         * Get the refresh rate of the current display.
         * @return - Refresh-rate, in hertz (Hz)
         */
        int refresh_rate();
    }
    /**
     * Convert a normalised-screen-space position into a pixel-screen-space position.
     * @param normalised_screen_space - The position between {0, 0} and {1.0f, 1.0f}
     * @param resolution - Resolution through which to convert
     * @return - The transformed position in pixel-screen-space
     */
    Vector2<int> to_pixel_screen_space(const Vector2F& normalised_screen_space, const Vector2<int>& resolution = display::resolution());
    /**
     * Convert a pixel-screen-space position into a normalised-screen-space position.
     * @param pixel_screen_space - The pixel position, such as {800, 600}
     * @param resolution - Resolution through which to convert
     * @return - The transformed position in normalised-screen-space (between {0, 0} and {1.0f, 1.0f})
     */
    Vector2F to_normalised_screen_space(const Vector2<int>& pixel_screen_space, const Vector2<int>& resolution = display::resolution());
    /**
     * Clamp a pixel-screen-space position, to ensure that it is valid. See the following example:
     *      Example Resolution = 800x600
     *      Input = {900, 1024} (isn't on the screen)
     *      Output = {800, 600} (clamps to maximum)
     * @param pixel_screen_space
     * @param resolution
     * @return
     */
    Vector2<int> clamp_pixel_screen_space(const Vector2<int>& pixel_screen_space, const Vector2<int>& resolution = display::resolution());
    /**
     * Generate the expected GUI quad (where {0, 0} modelspace is the bottom-left of the quad, instead of the middle)
     * @return - OpenGL quad, in a slightly manipulated format designed to make GUI simpler.
     */
    Mesh gui_quad();
}

#include "gui.inl"

#endif //TOPAZ_GUI_HPP
