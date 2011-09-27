#include <iostream>
#include <string>
#include <Magick++.h>

using namespace std;

int main(int /*argc*/,char **/*argv*/)
{
	try
	{
		string id = "316005574";
		unsigned int img_width = id.length()* 6;
		Magick::Image render(Magick::Geometry(img_width, 15), Magick::Color("white"));
		render.strokeColor("red");
		render.fillColor(Magick::ColorRGB(11, 87, 228));
		//render.font("Sans");
		render.fontPointsize(10);
		Magick::ColorRGB pix_color;
	double r, g, b;
	//uint32 offset = (MAP_WIDTH*start_y + start_x)*MAP_DEPTH;
	//uint8 *curr_pos = ((uint8 *) out_array) + DynamicArray::header_size() + offset;
	//uint32 jump_size = MAP_WIDTH - MAP_DEPTH*img_width;
	/*char single_pix;  
	// first copy out background of ROI
	for(uint32 y=0; y < 10; ++y)
	{
		for(uint32 x=0; x < img_width; ++x)
		{
		   // "safe" casting is time consuming and expensive
		   // TODO: look for a better way to do this! 
		   single_pix = *curr_pos;
		   // range of pixel = 0.0...1.0
		   r = static_cast<double>(single_pix) / 255.0;
		   ++curr_pos;
		   
		   single_pix = *curr_pos;
		   g = static_cast<double>(single_pix) / 255.0;;
		   ++curr_pos;
		   
		   single_pix = *curr_pos;
		   b = static_cast<double>(single_pix) / 255.0;
		   ++curr_pos;
		   
		   pix_color = Magick::ColorRGB(r, g, b);
		   render.pixelColor(x, y, pix_color);	   
		}
		curr_pos += jump_size;
	}
	*/
	render.draw(Magick::DrawableText(0,10, id));
	//Magick::Pixels view(render);
	//Magick::PixelPacket *pixels = view.get(0,0, img_width, 10);
	/*curr_pos = ((uint8 *) out_array) + DynamicArray::header_size() + offset;
	double pix_sample;
	// copy it back to the map
	for(uint32 y=0; y < 10; ++y)
	{
		for(uint32 x=0; x < img_width; ++x)
		{
			pix_color = *pixels;
			pix_sample = pix_color.red()*255.0;
			*curr_pos = static_cast<uint8>(pix_sample);		
			++curr_pos;
			
			pix_sample = pix_color.green()*255.0;
			*curr_pos = static_cast<uint8>(pix_sample);		
			++curr_pos;
			
			pix_sample = pix_color.blue()*255.0;
			*curr_pos = static_cast<uint8>(pix_sample);		
			++curr_pos;
			
			++pixels;
		}
		curr_pos += jump_size;
	}*/
	render.display();
	}
	catch( exception &error_ )
	{
		string my_error = error_.what();
		cout << "Caught exception: " <<  my_error;
		return 1;
	
	}
	return 0;
	}
	