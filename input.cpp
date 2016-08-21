struct SinglePress
{
	b4 pressed;
	f4 state;
	b4 released;
};

struct MYINPUT
{    
	SinglePress up, down, left, right;
	SinglePress restart;
	SinglePress enter, tab;
	SinglePress w,a,s,d;
	SinglePress spacebar,backspace;
	b4 mouse_clicked;
	b4 mouse_right_clicked;
	b4 display_console;
	b4 editor_mode;
	b4 hole_editor;
	b4 quit_app;
	b4 paused;
} input;

b4 single_press(SinglePress &key)
{
	if (key.released)
	{
		if (key.pressed)
		{
			key.state = true;
			key.released = false; 
		}
	}
	else
	{
		key.state = false;
		if (!key.pressed)
		{
			key.released = true;
		}
	} 
}

inline void poll_events()
{
	SDL_Event e;		
	while( SDL_PollEvent( &e ) != 0 )
	{
		if( e.type == SDL_MOUSEMOTION )
		{
		}
		else if ( e.type == SDL_MOUSEBUTTONDOWN )
		{
		}
		else if ( e.type == SDL_MOUSEBUTTONUP )
		{
			if ( e.button.button == SDL_BUTTON_LEFT )
			{
				input.mouse_clicked = true;
			}
			else if ( e.button.button == SDL_BUTTON_RIGHT )
			{
				input.mouse_right_clicked = true;
			}
		}
		else if( e.type == SDL_KEYDOWN )
		{
			switch( e.key.keysym.sym )
			{
				case SDLK_DOWN: { input.down.pressed = true; } break;
				case SDLK_UP: { input.up.pressed = true; } break;
				case SDLK_RIGHT: { input.right.pressed = true; } break;
				case SDLK_LEFT: { input.left.pressed = true; } break;
				case SDLK_d: { input.d.pressed = true; } break;
				case SDLK_a: { input.a.pressed = true; } break;
				case SDLK_ESCAPE: { input.quit_app = true; } break;
				case SDLK_SPACE: { input.spacebar.pressed = true; } break;
				default: {} break;
			}
		}
		else if( e.type == SDL_KEYUP )
		{
			switch( e.key.keysym.sym )
			{ 
				case SDLK_DOWN: { input.down.pressed = false;  } break;
				case SDLK_UP: { input.up.pressed = false;  } break;
				case SDLK_RIGHT: { input.right.pressed = false;  } break;
				case SDLK_LEFT: { input.left.pressed = false;  } break; 
				case SDLK_d: { input.d.pressed = false; } break;
				case SDLK_a: { input.a.pressed = false; } break;
				case SDLK_RETURN: { input.enter.pressed = false; } break;
				case SDLK_SPACE: { input.spacebar.pressed = false; } break;
				default: {} break;
			}
		} 
		else if( e.type == SDL_QUIT )
		{
			input.quit_app = true;
		}
	}  
}