#include <app/MidiWidget.hpp>
#include <ui/MenuSeparator.hpp>
#include <helpers.hpp>


namespace rack {
namespace app {


struct MidiDriverValueItem : ui::MenuItem {
	midi::Port* port;
	int driverId;
	void onAction(const ActionEvent& e) override {
		port->setDriverId(driverId);
	}
};

static void appendMidiDriverMenu(ui::Menu* menu, midi::Port* port) {
	if (!port)
		return;

	for (int driverId : midi::getDriverIds()) {
		MidiDriverValueItem* item = new MidiDriverValueItem;
		item->port = port;
		item->driverId = driverId;
		item->text = midi::getDriver(driverId)->getName();
		item->rightText = CHECKMARK(item->driverId == port->getDriverId());
		menu->addChild(item);
	}
}

struct MidiDriverChoice : LedDisplayChoice {
	midi::Port* port;
	void onAction(const ActionEvent& e) override {
		ui::Menu* menu = createMenu();
		menu->addChild(createMenuLabel("MIDI driver"));
		appendMidiDriverMenu(menu, port);
	}
	void step() override {
		text = (port && port->driver) ? port->getDriver()->getName() : "";
		if (text.empty()) {
			text = "(No driver)";
			color.a = 0.5f;
		}
		else {
			color.a = 1.f;
		}
	}
};

struct MidiDriverItem : ui::MenuItem {
	midi::Port* port;
	ui::Menu* createChildMenu() override {
		ui::Menu* menu = new ui::Menu;
		appendMidiDriverMenu(menu, port);
		return menu;
	}
};


struct MidiDeviceValueItem : ui::MenuItem {
	midi::Port* port;
	int deviceId;
	void onAction(const ActionEvent& e) override {
		port->setDeviceId(deviceId);
	}
};

static void appendMidiDeviceMenu(ui::Menu* menu, midi::Port* port) {
	if (!port)
		return;

	{
		MidiDeviceValueItem* item = new MidiDeviceValueItem;
		item->port = port;
		item->deviceId = -1;
		item->text = "(No device)";
		item->rightText = CHECKMARK(item->deviceId == port->getDeviceId());
		menu->addChild(item);
	}

	for (int deviceId : port->getDeviceIds()) {
		MidiDeviceValueItem* item = new MidiDeviceValueItem;
		item->port = port;
		item->deviceId = deviceId;
		item->text = port->getDeviceName(deviceId);
		item->rightText = CHECKMARK(item->deviceId == port->getDeviceId());
		menu->addChild(item);
	}
}

struct MidiDeviceChoice : LedDisplayChoice {
	midi::Port* port;
<<<<<<< HEAD
	int chosenDeviceId;
	void onAction(const event::Action& e) override {
		if (!port)
			return;

=======
	void onAction(const ActionEvent& e) override {
>>>>>>> 042a9ce026d253700ea14e340182900162ab4653
		ui::Menu* menu = createMenu();
		menu->addChild(createMenuLabel("MIDI device"));
		appendMidiDeviceMenu(menu, port);
	}
	void step() override {
<<<<<<< HEAD
		// cache the device name until the deviceId changes.
		if (!text.empty() && port && chosenDeviceId == port->deviceId)
			return;

		text = port ? port->getDeviceName(port->deviceId) : "";
=======
		text = (port && port->device) ? port->getDevice()->getName() : "";
>>>>>>> 042a9ce026d253700ea14e340182900162ab4653
		if (text.empty()) {
			text = "(No device)";
			chosenDeviceId = NULL;
			color.a = 0.5f;
		}
		else {
			chosenDeviceId = port->deviceId;
			color.a = 1.f;
		}
	}
};

struct MidiDeviceItem : ui::MenuItem {
	midi::Port* port;
	ui::Menu* createChildMenu() override {
		ui::Menu* menu = new ui::Menu;
		appendMidiDeviceMenu(menu, port);
		return menu;
	}
};


struct MidiChannelValueItem : ui::MenuItem {
	midi::Port* port;
	int channel;
	void onAction(const ActionEvent& e) override {
		port->setChannel(channel);
	}
};

static void appendMidiChannelMenu(ui::Menu* menu, midi::Port* port) {
	if (!port)
		return;

	for (int channel : port->getChannels()) {
		MidiChannelValueItem* item = new MidiChannelValueItem;
		item->port = port;
		item->channel = channel;
		item->text = port->getChannelName(channel);
		item->rightText = CHECKMARK(item->channel == port->getChannel());
		menu->addChild(item);
	}
}

struct MidiChannelChoice : LedDisplayChoice {
	midi::Port* port;
	void onAction(const ActionEvent& e) override {
		ui::Menu* menu = createMenu();
		menu->addChild(createMenuLabel("MIDI channel"));
		appendMidiChannelMenu(menu, port);
	}
	void step() override {
		text = port ? port->getChannelName(port->getChannel()) : "Channel 1";
	}
};

struct MidiChannelItem : ui::MenuItem {
	midi::Port* port;
	ui::Menu* createChildMenu() override {
		ui::Menu* menu = new ui::Menu;
		appendMidiChannelMenu(menu, port);
		return menu;
	}
};


void MidiWidget::setMidiPort(midi::Port* port) {
	clearChildren();

	math::Vec pos;

	MidiDriverChoice* driverChoice = createWidget<MidiDriverChoice>(pos);
	driverChoice->box.size.x = box.size.x;
	driverChoice->port = port;
	addChild(driverChoice);
	pos = driverChoice->box.getBottomLeft();
	this->driverChoice = driverChoice;

	this->driverSeparator = createWidget<LedDisplaySeparator>(pos);
	this->driverSeparator->box.size.x = box.size.x;
	addChild(this->driverSeparator);

	MidiDeviceChoice* deviceChoice = createWidget<MidiDeviceChoice>(pos);
	deviceChoice->box.size.x = box.size.x;
	deviceChoice->port = port;
	addChild(deviceChoice);
	pos = deviceChoice->box.getBottomLeft();
	this->deviceChoice = deviceChoice;

	this->deviceSeparator = createWidget<LedDisplaySeparator>(pos);
	this->deviceSeparator->box.size.x = box.size.x;
	addChild(this->deviceSeparator);

	MidiChannelChoice* channelChoice = createWidget<MidiChannelChoice>(pos);
	channelChoice->box.size.x = box.size.x;
	channelChoice->port = port;
	addChild(channelChoice);
	this->channelChoice = channelChoice;
}


void MidiButton::setMidiPort(midi::Port* port) {
	this->port = port;
}


void MidiButton::onAction(const ActionEvent& e) {
	ui::Menu* menu = createMenu();
	appendMidiMenu(menu, port);
}


void appendMidiMenu(ui::Menu* menu, midi::Port* port) {
	menu->addChild(createMenuLabel("MIDI driver"));
	appendMidiDriverMenu(menu, port);

	menu->addChild(new ui::MenuSeparator);
	menu->addChild(createMenuLabel("MIDI device"));
	appendMidiDeviceMenu(menu, port);

	menu->addChild(new ui::MenuSeparator);
	// menu->addChild(createMenuLabel("MIDI channel"));
	// appendMidiChannelMenu(menu, port);

	// Uncomment this to use sub-menus instead of one big menu.

	// MidiDriverItem* driverItem = createMenuItem<MidiDriverItem>("MIDI driver", RIGHT_ARROW);
	// driverItem->port = port;
	// menu->addChild(driverItem);

	// MidiDeviceItem* deviceItem = createMenuItem<MidiDeviceItem>("MIDI device", RIGHT_ARROW);
	// deviceItem->port = port;
	// menu->addChild(deviceItem);

	MidiChannelItem* channelItem = createMenuItem<MidiChannelItem>("MIDI channel", RIGHT_ARROW);
	channelItem->port = port;
	menu->addChild(channelItem);
}


} // namespace app
} // namespace rack
