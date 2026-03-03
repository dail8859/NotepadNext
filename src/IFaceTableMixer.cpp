/*
 * This file is part of Notepad Next.
 * Copyright 2019 Justin Dailey
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Notepad Next is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.
 */


#include "IFaceTableMixer.h"

const IFaceConstant *IFaceTableMixer::FindConstant(const char *name) const {
	for (auto const &iface : ifaces) {
		const IFaceConstant *ret = iface->FindConstant(name);
		if (ret != nullptr) return ret;
	}
	return nullptr;
}

const IFaceFunction *IFaceTableMixer::FindFunction(const char *name) const {
	for (auto const &iface : ifaces) {
		const IFaceFunction *ret = iface->FindFunction(name);
		if (ret != nullptr) return ret;
	}
	return nullptr;
}

const IFaceFunction *IFaceTableMixer::FindFunctionByConstantName(const char *name) const {
	for (auto const &iface : ifaces) {
		const IFaceFunction *ret = iface->FindFunctionByConstantName(name);
		if (ret != nullptr) return ret;
	}
	return nullptr;
}

const IFaceFunction *IFaceTableMixer::FindFunctionByValue(int value) const {
	for (auto const &iface : ifaces) {
		const IFaceFunction *ret = iface->FindFunctionByValue(value);
		if (ret != nullptr) return ret;
	}
	return nullptr;
}

const IFaceProperty *IFaceTableMixer::FindProperty(const char *name) const {
	for (auto const &iface : ifaces) {
		const IFaceProperty *ret = iface->FindProperty(name);
		if (ret != nullptr) return ret;
	}
	return nullptr;
}

int IFaceTableMixer::GetConstantName(int value, char *nameOut, unsigned nameBufferLen, const char *hint) const {
	for (auto const &iface : ifaces) {
		int ret = iface->GetConstantName(value, nameOut, nameBufferLen, hint);
		if (ret != 0) return ret;
	}
	return 0;
}

const IFaceFunction *IFaceTableMixer::GetFunctionByMessage(int message) const {
	for (auto const &iface : ifaces) {
		const IFaceFunction *ret = iface->GetFunctionByMessage(message);
		if (ret != nullptr) return ret;
	}
	return nullptr;
}

IFaceFunction IFaceTableMixer::GetPropertyFuncByMessage(int message) const {
	for (auto const &iface : ifaces) {
		IFaceFunction ret = iface->GetPropertyFuncByMessage(message);
		if (ret.value != -1) return ret;
	}
	return{ "invalid", -1, iface_void, { iface_void, iface_void } };
}
