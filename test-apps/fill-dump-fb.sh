#! /bin/sh

if [ ! -f ./draw_fb ]; then 
	echo "building draw_fb..."
	make
fi
if [ ! -f ./draw_fb ]; then 
	echo "building errors, exiting..."
	exit 1	
fi

echo "filling /dev/fb2 with yellow..."
./draw_fb -i /dev/fb2 -c yellow & 
echo "filling /dev/fb3 with green..."
./draw_fb -i /dev/fb3 -c green &
echo "filling /dev/fb4 with blue..."
./draw_fb -i /dev/fb4 -c blue &
echo "filling /dev/fb5 with red..."
./draw_fb -i /dev/fb5 -c red &

echo "dumping fb1 content to out.avi..."
ffmpeg -f fbdev -i /dev/fb1 out.avi
