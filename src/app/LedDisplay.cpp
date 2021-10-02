#include <app/LedDisplay.hpp>
#include <asset.hpp>
#include <window/Window.hpp>
#include <context.hpp>


namespace rack {
namespace app {


void LedDisplay::draw(const DrawArgs& args) {
	nvgBeginPath(args.vg);
	nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 5.0);
	nvgFillColor(args.vg, nvgRGB(0x00, 0x00, 0x00));
	nvgFill(args.vg);

	nvgScissor(args.vg, RECT_ARGS(args.clipBox));
	Widget::draw(args);
	nvgResetScissor(args.vg);
}


LedDisplaySeparator::LedDisplaySeparator() {
	box.size = math::Vec();
}


void LedDisplaySeparator::draw(const DrawArgs& args) {
	nvgBeginPath(args.vg);
	nvgMoveTo(args.vg, 0, 0);
	nvgLineTo(args.vg, box.size.x, box.size.y);
	nvgStrokeWidth(args.vg, 1.0);
	nvgStrokeColor(args.vg, nvgRGB(0x33, 0x33, 0x33));
	nvgStroke(args.vg);
}


LedDisplayChoice::LedDisplayChoice() {
	box.size = window::mm2px(math::Vec(0, 28.0 / 3));
	fontPath = asset::system("res/fonts/ShareTechMono-Regular.ttf");
	textOffset = math::Vec(10, 18);
	color = nvgRGB(0xff, 0xd7, 0x14);
	bgColor = nvgRGBAf(0, 0, 0, 0);
}


void LedDisplayChoice::draw(const DrawArgs& args) {
	nvgScissor(args.vg, RECT_ARGS(args.clipBox));
	if (bgColor.a > 0.0) {
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
		nvgFillColor(args.vg, bgColor);
		nvgFill(args.vg);
	}

	std::shared_ptr<window::Font> font = APP->window->loadFont(fontPath);
	nvgGlobalTint(args.vg, color::WHITE);
	if (font && font->handle >= 0) {
		nvgFillColor(args.vg, color);
		nvgFontFaceId(args.vg, font->handle);
		nvgTextLetterSpacing(args.vg, 0.0);

		nvgFontSize(args.vg, 12);
		nvgText(args.vg, textOffset.x, textOffset.y, text.c_str(), NULL);
	}
	nvgResetScissor(args.vg);
}


void LedDisplayChoice::onButton(const ButtonEvent& e) {
	OpaqueWidget::onButton(e);

	if (e.action == GLFW_PRESS && (e.button == GLFW_MOUSE_BUTTON_LEFT || e.button == GLFW_MOUSE_BUTTON_RIGHT)) {
		ActionEvent eAction;
		onAction(eAction);
		e.consume(this);
	}
}


LedDisplayTextField::LedDisplayTextField() {
	fontPath = asset::system("res/fonts/ShareTechMono-Regular.ttf");
	textOffset = math::Vec(5, 5);
	color = nvgRGB(0xff, 0xd7, 0x14);
	bgColor = nvgRGB(0x00, 0x00, 0x00);
}


void LedDisplayTextField::draw(const DrawArgs& args) {
	nvgScissor(args.vg, RECT_ARGS(args.clipBox));

	// Background
	if (bgColor.a > 0.0) {
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 5.0);
		nvgFillColor(args.vg, nvgRGB(0x00, 0x00, 0x00));
		nvgFill(args.vg);
	}

	// Text
	std::shared_ptr<window::Font> font = APP->window->loadFont(fontPath);
	nvgGlobalTint(args.vg, color::WHITE);
	if (font && font->handle >= 0) {
		bndSetFont(font->handle);

		NVGcolor highlightColor = color;
		highlightColor.a = 0.5;
		int begin = std::min(cursor, selection);
		int end = (this == APP->event->selectedWidget) ? std::max(cursor, selection) : -1;
		bndIconLabelCaret(args.vg,
			textOffset.x, textOffset.y,
			box.size.x - 2 * textOffset.x, box.size.y - 2 * textOffset.y,
			-1, color, 12, text.c_str(), highlightColor, begin, end);

		bndSetFont(APP->window->uiFont->handle);
	}

	nvgResetScissor(args.vg);
}


int LedDisplayTextField::getTextPosition(math::Vec mousePos) {
	std::shared_ptr<window::Font> font = APP->window->loadFont(fontPath);
	if (!font || !font->handle)
		return 0;

	bndSetFont(font->handle);
	int textPos = bndIconLabelTextPosition(APP->window->vg,
		textOffset.x, textOffset.y,
		box.size.x - 2 * textOffset.x, box.size.y - 2 * textOffset.y,
		-1, 12, text.c_str(), mousePos.x, mousePos.y);
	bndSetFont(APP->window->uiFont->handle);
	return textPos;
}


} // namespace app
} // namespace rack
