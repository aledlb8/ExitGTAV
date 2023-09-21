#pragma once

namespace Gui
{
	void	Init( HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* device_context );
	void	Customize( void );
	void	Think(void);
	void	Render( void );
	void	Unload( void );

	inline float TitleBarHeight = NULL;
	inline float CloseButtonWidth = NULL;
	inline bool IsWindowCollapsed = false;
}