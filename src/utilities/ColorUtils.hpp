#include "imgui.h"
#include "math.h"
#include <utility>

namespace ColorUtils{
	/**
	 * @brief Compares two ImGui colors based on their brightness.
	 *
	 * This function is designed to be used with sorting algorithms (like std::sort)
	 * to arrange a collection of colors from darkest to brightest.
	 *
	 * @param color1 The first color to compare.
	 * @param color2 The second color to compare.
	 * @return Returns true if color1 is darker than color2; otherwise, false.
	 */
	inline bool CompareImColors(const ImColor& color1, const ImColor& color2) {
	    ImVec4 col1 = color1.Value;
	    ImVec4 col2 = color2.Value;

	    float h1, s1, v1; // For color1
	    float h2, s2, v2; // For color2

	    // The 'Value' (V) component of HSV represents the color's brightness.
	    ImGui::ColorConvertRGBtoHSV(col1.x, col1.y, col1.z, h1, s1, v1);
	    ImGui::ColorConvertRGBtoHSV(col2.x, col2.y, col2.z, h2, s2, v2);

	    // Compare the brightness (Value) of the two colors.
	    return v1 < v2;
	}

	/**
	 * @brief Calculates the relative luminance of a color component (R, G, or B).
	 * The formula is based on the WCAG standard for sRGB.
	 */
	inline float GetLuminance(float val) {
	    if (val <= 0.03928f) {
	        return val / 12.92f;
	    }
	    return powf((val + 0.055f) / 1.055f, 2.4f);
	}

	/**
	 * @brief Calculates the contrast ratio between two colors.
	 * A ratio of 4.5 is the minimum recommended for normal text (WCAG AA).
	 * @param color1 The first color.
	 * @param color2 The second color.
	 * @return The contrast ratio (e.g., 4.5f for 4.5:1).
	 */
	inline float GetContrastRatio(const ImVec4& color1, const ImVec4& color2) {
	    // Calculate the relative luminance for each color
	    float lum1 = 0.2126f * GetLuminance(color1.x) + 0.7152f * GetLuminance(color1.y) + 0.0722f * GetLuminance(color1.z);
	    float lum2 = 0.2126f * GetLuminance(color2.x) + 0.7152f * GetLuminance(color2.y) + 0.0722f * GetLuminance(color2.z);

	    // Ensure lum1 is the lighter color
	    if (lum1 < lum2) {
	        std::swap(lum1, lum2);
	    }

	    return (lum1 + 0.05f) / (lum2 + 0.05f);
	}
	
}
