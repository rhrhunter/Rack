#include <app/ModuleLightWidget.hpp>
#include <app/Scene.hpp>
#include <context.hpp>
#include <settings.hpp>


namespace rack {
namespace app {


struct LightTooltip : ui::Tooltip {
	ModuleLightWidget* lightWidget;

	void step() override {
		if (lightWidget->module) {
			engine::LightInfo* lightInfo = lightWidget->getLightInfo();
			if (!lightInfo)
				return;
			// Label
			text = lightInfo->getName();
			text += " light";
			// Description
			std::string description = lightInfo->getDescription();
			if (description != "") {
				text += "\n";
				text += description;
			}
			// Brightness for each color
			text += "\n";
			int numColors = lightWidget->getNumColors();
			for (int colorId = 0; colorId < numColors; colorId++) {
				if (colorId > 1)
					text += " ";
				engine::Light* light = lightWidget->getLight(colorId);
				float brightness = math::clamp(light->getBrightness(), 0.f, 1.f);
				text += string::f("% 3.0f%%", brightness * 100.f);
			}
		}
		Tooltip::step();
		// Position at bottom-right of parameter
		box.pos = lightWidget->getAbsoluteOffset(lightWidget->box.size).round();
		// Fit inside parent (copied from Tooltip.cpp)
		assert(parent);
		box = box.nudge(parent->box.zeroPos());
	}
};


ModuleLightWidget::~ModuleLightWidget() {
	destroyTooltip();
}


engine::Light* ModuleLightWidget::getLight(int colorId) {
	if (!module)
		return NULL;
	if (firstLightId < 0)
		return NULL;
	return &module->lights[firstLightId + colorId];
}


engine::LightInfo* ModuleLightWidget::getLightInfo() {
	if (!module)
		return NULL;
	if (firstLightId < 0)
		return NULL;
	return module->lightInfos[firstLightId];
}


void ModuleLightWidget::createTooltip() {
	if (!settings::tooltips)
		return;
	if (this->tooltip)
		return;
	// If the LightInfo is null, don't show a tooltip
	if (!getLightInfo())
		return;
	LightTooltip* tooltip = new LightTooltip;
	tooltip->lightWidget = this;
	APP->scene->addChild(tooltip);
	this->tooltip = tooltip;
}


void ModuleLightWidget::destroyTooltip() {
	if (!tooltip)
		return;
	APP->scene->removeChild(tooltip);
	delete tooltip;
	tooltip = NULL;
}


void ModuleLightWidget::step() {
	std::vector<float> brightnesses(baseColors.size());

	if (module && firstLightId >= 0) {
		assert((int) module->lights.size() >= firstLightId + (int) baseColors.size());

		for (size_t i = 0; i < baseColors.size(); i++) {
			float b = module->lights[firstLightId + i].getBrightness();
			if (!std::isfinite(b))
				b = 0.f;
			b = math::clamp(b, 0.f, 1.f);
			// Because LEDs are nonlinear, this seems to look more natural.
			b = std::sqrt(b);
			brightnesses[i] = b;
		}
	}
	else {
		// Turn all lights on
		for (size_t i = 0; i < baseColors.size(); i++) {
			brightnesses[i] = 1.f;
		}
	}

	setBrightnesses(brightnesses);

	MultiLightWidget::step();
}


void ModuleLightWidget::onHover(const HoverEvent& e) {
	// Adapted from OpaqueWidget::onHover()
	Widget::onHover(e);

	// If the LightInfo is null, don't consume Hover event
	if (!getLightInfo())
		return;

	e.stopPropagating();
	// Consume if not consumed by child
	if (!e.isConsumed())
		e.consume(this);
}


void ModuleLightWidget::onEnter(const EnterEvent& e) {
	createTooltip();
}


void ModuleLightWidget::onLeave(const LeaveEvent& e) {
	destroyTooltip();
}


} // namespace app
} // namespace rack
