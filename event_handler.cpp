#include "event_handler.h"

Event::Event(int event_type_, int event_number_, int player_, char* data_, int size_, bool domemcpy)
{
	player = player_;
	event_type = event_type_;
	event_number = event_number_;
	size = size_;
	
	if(domemcpy)
	{
		data = (char*)malloc(size);
		memcpy(data, data_, size);
	}
	else
		data = data_;
}

Event::~Event()
{
	free(data);
}



