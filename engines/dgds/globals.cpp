/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "dgds/globals.h"
#include "dgds/dgds.h"
#include "dgds/scene.h"

namespace Dgds {


typedef ReadOnlyGlobal<int16> ROI16Global;
typedef ReadWriteGlobal<int16> RWI16Global;

////////////////////////////////

// TODO: Move this to Scene??
class GameIsInteractiveGlobal : public Global {
public:
	GameIsInteractiveGlobal(uint16 num, int16 *ptr) : Global(num), _ptr(ptr), _isSetOff(false) {}

	int16 get() override {
		SDSScene *scene = DgdsEngine::getInstance()->getScene();
		bool nonInteractive = _isSetOff || scene->getDragItem() || scene->hasVisibleOrOpeningDialog();
		*_ptr = !nonInteractive;
		return *_ptr;
	}

	int16 set(int16 val) override {
		_isSetOff = (val == 0);
		return get();
	}

	void setRaw(int16 val) override { }

private:
	int16 *_ptr;
	bool _isSetOff;
};


Globals::Globals(Clock &clock) :
_lastOpcode1SceneChageNum(0), _sceneOp12SceneNum(0), _currentSelectedItem(0),
_gameMinsToAddOnLClick(0), _gameMinsToAddOnStartDrag(0), _gameMinsToAddOnRClick(0), _gameMinsToAddOnDragFinished(0),
_gameMinsToAddOnObjInteraction(0), _gameIsInteractiveGlobal(0), _sceneOpcode15FromScene(0),
_sceneOpcode15ToScene(0) {
	_globals.push_back(clock.getGameMinsAddedGlobal(1));
	_globals.push_back(clock.getGameTicksUpGlobal(0x64));
	_globals.push_back(clock.getGameTicksDownGlobal(0x63));
	_globals.push_back(new ROI16Global(0x62, &_lastOpcode1SceneChageNum));
	_globals.push_back(new RWI16Global(0x61, &_sceneOp12SceneNum));
	_globals.push_back(new RWI16Global(0x60, &_currentSelectedItem));
	_globals.push_back(clock.getDaysGlobal(0x5F));
	_globals.push_back(clock.getHoursGlobal(0x5E));
	_globals.push_back(clock.getMinsGlobal(0x5D));
	_globals.push_back(new RWI16Global(0x5C, &_gameMinsToAddOnLClick));
	_globals.push_back(new RWI16Global(0x5B, &_gameMinsToAddOnStartDrag));
	_globals.push_back(new RWI16Global(0x5A, &_gameMinsToAddOnRClick));
	_globals.push_back(new RWI16Global(0x59, &_gameMinsToAddOnDragFinished));
	_globals.push_back(new RWI16Global(0x58, &_gameMinsToAddOnObjInteraction));
	_globals.push_back(new GameIsInteractiveGlobal(0x57, &_gameIsInteractiveGlobal));
	_globals.push_back(clock.getDays2Global(0x56));
	_globals.push_back(new RWI16Global(0x55, &_sceneOpcode15FromScene));
	_globals.push_back(new RWI16Global(0x54, &_sceneOpcode15ToScene));
}

Globals::~Globals() {
	for (auto &g : _globals)
		delete g;
}

int16 Globals::getGlobal(uint16 num) {
	for (auto &global : _globals) {
		if (global->getNum() == num)
			return global->get();
	}

	if (num == 333) {
		// Bug in HoC (scene 21)?
		warning("getGlobal: requested global 333");
		return 0;
	}

	// This happens in a couple of places in RotD
	if (num)
		warning("getGlobal: requested non-existing global %d", num);

	// Bug in HoC?
	//warning("getGlobal: requested global 0");
	return 0;
}

int16 Globals::setGlobal(uint16 num, int16 val) {
	//debug(1, "setGlobal %d -> %d", num, val);
	for (auto &global : _globals) {
		if (global->getNum() == num)
			return global->set(val);
	}

	// This happens eg looking at the Fisto box in RotD
	warning("setGlobal: requested non-existing global %d", num);
	return 0;
}

Common::Error Globals::syncState(Common::Serializer &s) {
	s.syncAsSint16LE(_lastOpcode1SceneChageNum);
	s.syncAsSint16LE(_sceneOp12SceneNum);
	s.syncAsSint16LE(_currentSelectedItem);
	s.syncAsSint16LE(_gameMinsToAddOnLClick);
	s.syncAsSint16LE(_gameMinsToAddOnStartDrag);
	s.syncAsSint16LE(_gameMinsToAddOnRClick);
	s.syncAsSint16LE(_gameMinsToAddOnDragFinished);
	s.syncAsSint16LE(_gameMinsToAddOnObjInteraction);
	s.syncAsSint16LE(_gameIsInteractiveGlobal);
	s.syncAsSint16LE(_sceneOpcode15FromScene);
	s.syncAsSint16LE(_sceneOpcode15ToScene);
	return Common::kNoError;
}

////////////////////////////////

class DetailLevelROGlobal : public Global {
public:
	DetailLevelROGlobal(uint16 num) : Global(num) {}
	int16 get() override { return DgdsEngine::getInstance()->getDetailLevel(); }
	int16 set(int16 val) override { return DgdsEngine::getInstance()->getDetailLevel(); }
	void setRaw(int16 val) override { }
};

////////////////////////////////

static byte dragonDataTableOffsets[] = {0x9, 0x29, 0xF, 0x15, 0x5C, 0x19, 0x28, 0x1F};

static byte dragonDataTable[] = {
	 0x4,  0x8, 0x16,  0xE,  0x8,  0xE, 0x17, 0x1C,
	 0x8,  0x2, 0x18,  0x8, 0x10, 0x10, 0x17, 0x15,
	0x16, 0x18,  0x3, 0x20, 0x1E, 0x24, 0x32, 0x30,
	 0xE,  0x8, 0x20,  0x3, 0x13,  0xE, 0x14, 0x12,
	 0x8, 0x10, 0x1E, 0x13,  0x3,  0x8, 0x12, 0x22,
	 0xE, 0x10, 0x24,  0xE,  0x8,  0x3,  0xB, 0x20,
	0x17, 0x17, 0x32, 0x14, 0x12,  0xB,  0x4, 0x24,
	0x1C, 0x15, 0x30, 0x12, 0x22, 0x20, 0x24,  0x4
};

DragonDataTable::DragonDataTable() : _row(0), _col(0), _divBy4(0), _output(0) {}

int DragonDataTable::getOffsetForVal(uint16 val) const {
	for (int i = 0; i < ARRAYSIZE(dragonDataTableOffsets); i++) {
		if (dragonDataTableOffsets[i] == val)
			return i;
	}
	return 0;
}

uint16 DragonDataTable::getValueFromTable() {
	int row = getOffsetForVal(_row);
	int col = getOffsetForVal(_col);
	_output = dragonDataTable[row * 8 + col];
	if (_divBy4)
		_output /= 4;
	if (_output == 0)
		_output = 1;
	return _output;
}

class DragonDataTableGlobal : public Global {
public:
	DragonDataTableGlobal(uint16 num, DragonDataTable &table) : Global(num), _table(table) {}
	int16 get() override { return _table.getValueFromTable(); }
	int16 set(int16 val) override { return _table.getValueFromTable(); }
	void setRaw(int16 val) override { }
private:
	DragonDataTable &_table;
};


////////////////////////////////

DragonGlobals::DragonGlobals(Clock &clock) : Globals(clock),
 _sceneOpcode100Var(0), _arcadeState(0), _opcode106EndMinutes(0) {
	_globals.push_back(new RWI16Global(0x20, &_sceneOpcode100Var));
	_globals.push_back(new RWI16Global(0x21, &_arcadeState));
	_globals.push_back(new RWI16Global(0x22, &_opcode106EndMinutes));
	_globals.push_back(new RWI16Global(0x23, &_table._row));
	_globals.push_back(new RWI16Global(0x24, &_table._col));
	_globals.push_back(new RWI16Global(0x25, &_table._divBy4));
	_globals.push_back(new DragonDataTableGlobal(0x26, _table));
	_globals.push_back(new DetailLevelROGlobal(0x27));
}

Common::Error DragonGlobals::syncState(Common::Serializer &s) {
	Globals::syncState(s);
	s.syncAsSint16LE(_sceneOpcode100Var);
	s.syncAsSint16LE(_arcadeState);
	s.syncAsSint16LE(_opcode106EndMinutes);

	s.syncAsSint16LE(_table._row);
	s.syncAsSint16LE(_table._col);
	s.syncAsSint16LE(_table._divBy4);
	s.syncAsSint16LE(_table._output);

	return Common::kNoError;
}

class HocCharacterGlobal : public RWI16Global {
public:
	HocCharacterGlobal(uint16 num, int16 *val) : RWI16Global(num, val) {}
	int16 set(int16 val) override {
		DgdsEngine *engine = DgdsEngine::getInstance();
		bool buttonVisible = engine->isInvButtonVisible();
		if (buttonVisible)
			engine->getScene()->removeInvButtonFromHotAreaList();
		RWI16Global::set(val);
		if (buttonVisible)
			engine->getScene()->addInvButtonToHotAreaList();
		return get();
	}
};


HocGlobals::HocGlobals(Clock &clock) : Globals(clock), _difficultyLevel(1), _unk55(0),
	_unkDlgFileNum(0), _unkDlgDlgNum(0),  _currentCharacter2(0), _currentCharacter(0),
	_tankFinished(0), _nativeGameState(0), _tankState(0), _unk47(0), _unk46(0), _unk45(0x3f), _sheckels(0),
	_shellBet(0), _shellPea(0), _trainState(0), _startScene(3), _introState(0) {
	_globals.push_back(new DetailLevelROGlobal(0x53));
	_globals.push_back(new RWI16Global(0x52, &_difficultyLevel)); // TODO: Sync with difficulty in menu
	_globals.push_back(new RWI16Global(0x37, &_unk55)); // TODO: Special update function FUN_1407_080d, sound init related.. sound bank?
	_globals.push_back(new RWI16Global(0x36, &_unkDlgFileNum));
	_globals.push_back(new RWI16Global(0x35, &_unkDlgDlgNum));
	_globals.push_back(new HocCharacterGlobal(0x34, &_currentCharacter));
	_globals.push_back(new HocCharacterGlobal(0x33, &_currentCharacter2));
	_globals.push_back(new RWI16Global(0x32, &_tankFinished));
	_globals.push_back(new RWI16Global(0x31, &_nativeGameState));
	_globals.push_back(new RWI16Global(0x30, &_tankState));
	_globals.push_back(new RWI16Global(0x2F, &_unk47)); // tank related.. cows?
	_globals.push_back(new RWI16Global(0x2E, &_unk46)); // tank related.. start point?
	_globals.push_back(new RWI16Global(0x2D, &_unk45)); // TODO: Special update function FUN_1407_0784, palette related?
	_globals.push_back(new RWI16Global(0x2C, &_sheckels));	// used as currency in Istanbul
	_globals.push_back(new RWI16Global(0x2B, &_shellBet));
	_globals.push_back(new RWI16Global(0x2A, &_shellPea));
	_globals.push_back(new RWI16Global(0x29, &_trainState));
	_globals.push_back(new RWI16Global(0x28, &_startScene));
	_globals.push_back(new ROI16Global(0x27, &_introState));
}

Common::Error HocGlobals::syncState(Common::Serializer &s) {
	Globals::syncState(s);

	s.syncAsSint16LE(_introState);
	s.syncAsSint16LE(_startScene);
	s.syncAsSint16LE(_trainState);
	s.syncAsSint16LE(_shellPea);
	s.syncAsSint16LE(_shellBet);
	s.syncAsSint16LE(_sheckels);
	s.syncAsSint16LE(_unk45);
	s.syncAsSint16LE(_unk46);
	s.syncAsSint16LE(_unk47);
	s.syncAsSint16LE(_tankState);
	s.syncAsSint16LE(_nativeGameState);
	s.syncAsSint16LE(_tankFinished);
	s.syncAsSint16LE(_currentCharacter);
	s.syncAsSint16LE(_currentCharacter2);
	s.syncAsSint16LE(_unkDlgDlgNum);
	s.syncAsSint16LE(_unkDlgFileNum);
	s.syncAsSint16LE(_unk55);
	s.syncAsSint16LE(_difficultyLevel);

	return Common::kNoError;
}

WillyGlobals::WillyGlobals(Clock &clock) : Globals(clock),
	_unk2(4), _unk3(0), _unk4(0), _unk5(0), _unk74(0), _unk75(300),
	_unk77(255), _unk78(0), _unk79(0), _unk80(0), _unk81(3), _unk82(1) {
	_globals.push_back(new DetailLevelROGlobal(0x53));
	_globals.push_back(new RWI16Global(0x52, &_unk82));
	_globals.push_back(new RWI16Global(0x51, &_unk81));
	_globals.push_back(new RWI16Global(0x50, &_unk80));
	_globals.push_back(new RWI16Global(0x4F, &_unk79));
	_globals.push_back(new RWI16Global(0x4E, &_unk78));
	_globals.push_back(new RWI16Global(0x4D, &_unk77));
	_globals.push_back(new RWI16Global(0x4C, &_unk77)); // TODO: Special set function 1833:665e. Same variable as 0x4D.
	_globals.push_back(new RWI16Global(0x4B, &_unk75));
	_globals.push_back(new RWI16Global(0x4A, &_unk74));
	_globals.push_back(new RWI16Global(0x05, &_unk5));
	_globals.push_back(new RWI16Global(0x04, &_unk4));
	_globals.push_back(new RWI16Global(0x03, &_unk3));
	_globals.push_back(new RWI16Global(0x02, &_unk2)); // TODO: Special set function 1574:06ca
}

Common::Error WillyGlobals::syncState(Common::Serializer &s) {
	Globals::syncState(s);
	s.syncAsSint16LE(_unk2);
	s.syncAsSint16LE(_unk3);
	s.syncAsSint16LE(_unk4);
	s.syncAsSint16LE(_unk5);
	s.syncAsSint16LE(_unk74);
	s.syncAsSint16LE(_unk75);
	s.syncAsSint16LE(_unk77);
	s.syncAsSint16LE(_unk78);
	s.syncAsSint16LE(_unk79);
	s.syncAsSint16LE(_unk80);
	s.syncAsSint16LE(_unk81);
	s.syncAsSint16LE(_unk82);

	return Common::kNoError;
}

} // end namespace Dgds
