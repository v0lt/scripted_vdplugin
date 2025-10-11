//
// Copyright (c) 2025 v0lt
//
// SPDX-License-Identifier: MIT
//

#pragma once

class CWindowsDPI
{
public:
	inline static CWindowsDPI& GetInstance()
	{
		if (nullptr == m_pInstance.get()) {
			m_pInstance.reset(new CWindowsDPI());
		}
		return *m_pInstance;
	}

	UINT DpiForWindow(HWND hwnd) const;

	~CWindowsDPI();

protected:

private:
	CWindowsDPI();   // Private because singleton

	inline static std::unique_ptr<CWindowsDPI> m_pInstance;
};
