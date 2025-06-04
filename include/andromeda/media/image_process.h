#ifndef ANDROMEDA_MEDIA_IMAGEPROCESS
#define ANDROMEDA_MEDIA_IMAGEPROCESS

#define getPixelChannelFromBytePtr(ptr,cnl) ((int)((Pixel*)ptr)->cnl)

#define startTraversalRefPixelArea(ref_pixel,size_x,size_y,pos_x,pos_y)\
        {\
            int absolute_x,absolute_y,relative_x=0,relative_y=0,start_absolute_x,start_absolute_y,end_absolute_x,end_absolute_y,start_relative_x=0,start_relative_y=0,end_relative_x=size_x-1,end_relative_y=size_y-1,pixel_count=0;\
            start_absolute_x=ref_pixel.x-pos_x;\
            start_absolute_y=ref_pixel.y-pos_y;\
            if(start_absolute_x<0)\
            {\
                start_relative_x=-start_absolute_x;\
                start_absolute_x=0;\
            }\
            if(start_absolute_y<0)\
            {\
                start_relative_y=-start_absolute_y;\
                start_absolute_y=0;\
            }\
            end_absolute_x=ref_pixel.x-pos_x+size_x-1;\
            end_absolute_y=ref_pixel.y-pos_y+size_y-1;\
            if(end_absolute_x>=ref_pixel.getImageWidth())\
            {\
                end_relative_x=size_x-end_absolute_x+ref_pixel.getImageWidth()-2;\
                end_absolute_x=ref_pixel.getImageWidth()-1;\
            }\
            if(end_absolute_y>=ref_pixel.getImageHeight())\
            {\
                end_relative_y=size_y-end_absolute_y+ref_pixel.getImageHeight()-2;\
                end_absolute_y=ref_pixel.getImageWidth()-1;\
            }\
            absolute_y=start_absolute_y;\
            relative_y=start_relative_y;\
            LOOP_Y_IN_AREA_##ref_pixel##size_x##size_y##pos_x##pos_y:\
                relative_x=start_relative_x;\
                absolute_x=start_absolute_x;\
                LOOP_X_IN_AREA_##ref_pixel##size_x##size_y##pos_x##pos_y:\
                    Pixel &this_pixel_in_ref_area=ref_pixel.getPixelInImage(absolute_x,absolute_y);\
                    ++pixel_count;\
                    {

#define endTraversalRefPixelArea(ref_pixel,size_x,size_y,pos_x,pos_y)\
                    }\
                    ++relative_x;\
                    ++absolute_x;\
                if(relative_x<=end_relative_x)goto LOOP_X_IN_AREA_##ref_pixel##size_x##size_y##pos_x##pos_y;\
                ++relative_y;\
                ++absolute_y;\
            if(relative_y<=end_relative_y)goto LOOP_Y_IN_AREA_##ref_pixel##size_x##size_y##pos_x##pos_y;\
        }

#define startTraversalPixelsInArea(image,start_x,start_y,end_x,end_y,stride_x,stride_y,op)\
        {\
            register int WIDTH_OF_AREA_##image##start_x##start_y##end_x##end_y##stride_x##stride_y##op=end_x-start_x,HEIGHT_OF_AREA_##image##start_x##start_y##end_x##end_y##stride_x##stride_y##op=end_y-start_y,traversaled_count=0,absolute_x_in_area=start_x,absolute_y_in_area=start_y,relative_x_in_area=0,relative_y_in_area=0;\
            register image_edge_options exec_flag_x,exec_flag_y=op;\
            LOOP_Y_IN_AREA_##image##start_x##start_y##end_x##end_y##stride_x##stride_y##op:\
                exec_flag_x=op;\
                LOOP_X_IN_AREA_##image##start_x##start_y##end_x##end_y##stride_x##stride_y##op:\
                    Pixel &this_pixel_in_area=image.getPixel(absolute_x_in_area,absolute_y_in_area);\
                    {

#define endTraversalPixelsInArea(image,start_x,start_y,end_x,end_y,stride_x,stride_y,op)\
                    }\
                    ++traversaled_count;\
                    ++absolute_x_in_area+=stride_x;\
                    ++relative_x_in_area+=stride_x;\
                if(relative_x_in_area<WIDTH_OF_AREA_##image##start_x##start_y##end_x##end_y##stride_x##stride_y##op)goto LOOP_X_IN_AREA_##image##start_x##start_y##end_x##end_y##stride_x##stride_y##op;\
                switch(exec_flag_x)\
                {\
                case IMAGE_EDGE_INCLUDE:\
                    absolute_x_in_area=start_x+WIDTH_OF_AREA_##image##start_x##start_y##end_x##end_y##stride_x##stride_y##op-1;\
                    relative_x_in_area=WIDTH_OF_AREA_##image##start_x##start_y##end_x##end_y##stride_x##stride_y##op-1;\
                    exec_flag_x=IMAGE_EDGE_ABANDON;\
                    goto LOOP_X_IN_AREA_##image##start_x##start_y##end_x##end_y##stride_x##stride_y##op;\
                    break;\
                case IMAGE_EDGE_ABANDON:\
                    break;\
                }\
                absolute_x_in_area=start_x;\
                relative_x_in_area=0;\
                ++absolute_y_in_area+=stride_y;\
                ++relative_y_in_area+=stride_y;\
            if(relative_y_in_area<HEIGHT_OF_AREA_##image##start_x##start_y##end_x##end_y##stride_x##stride_y##op)goto LOOP_Y_IN_AREA_##image##start_x##start_y##end_x##end_y##stride_x##stride_y##op;\
            switch(exec_flag_y)\
            {\
            case IMAGE_EDGE_INCLUDE:\
                absolute_y_in_area=start_y+HEIGHT_OF_AREA_##image##start_x##start_y##end_x##end_y##stride_x##stride_y##op-1;\
                relative_y_in_area=HEIGHT_OF_AREA_##image##start_x##start_y##end_x##end_y##stride_x##stride_y##op-1;\
                exec_flag_y=IMAGE_EDGE_ABANDON;\
                goto LOOP_Y_IN_AREA_##image##start_x##start_y##end_x##end_y##stride_x##stride_y##op;\
                break;\
            case IMAGE_EDGE_ABANDON:\
                break;\
            }\
        }

#define startTraversalPixels(image,x,y,stride_x,stride_y,op)\
        {\
            register int WIDTH_OF_IMAGE_##image##x##y##stride_x##stride_y##op=image.width,HEIGHT_OF_IMAGE_##image##x##y##stride_x##stride_y##op=image.height,traversaled_count=0,absolute_x=x,absolute_y=y;\
            register image_edge_options exec_flag_x,exec_flag_y=op;\
            LOOP_Y_IN_IMAGE_##image##x##y##stride_x##stride_y##op:\
                exec_flag_x=op;\
                LOOP_X_IN_IMAGE_##image##x##y##stride_x##stride_y##op:\
                    Pixel &this_pixel_in_image=image.getPixel(absolute_x,absolute_y);\
                    {

#define endTraversalPixels(image,x,y,stride_x,stride_y,op)\
                    }\
                    ++traversaled_count;\
                    ++absolute_x+=stride_x;\
                if(absolute_x<WIDTH_OF_IMAGE_##image##x##y##stride_x##stride_y##op)goto LOOP_X_IN_IMAGE_##image##x##y##stride_x##stride_y##op;\
                switch(exec_flag_x)\
                {\
                case IMAGE_EDGE_INCLUDE:\
                    absolute_x=WIDTH_OF_IMAGE_##image##x##y##stride_x##stride_y##op-1;\
                    exec_flag_x=IMAGE_EDGE_ABANDON;\
                    goto LOOP_X_IN_IMAGE_##image##x##y##stride_x##stride_y##op;\
                    break;\
                case IMAGE_EDGE_ABANDON:\
                    break;\
                }\
                absolute_x=0;\
                ++absolute_y+=stride_y;\
            if(absolute_y<HEIGHT_OF_IMAGE_##image##x##y##stride_x##stride_y##op)goto LOOP_Y_IN_IMAGE_##image##x##y##stride_x##stride_y##op;\
            switch(exec_flag_y)\
            {\
            case IMAGE_EDGE_INCLUDE:\
                absolute_y=HEIGHT_OF_IMAGE_##image##x##y##stride_x##stride_y##op-1;\
                exec_flag_y=IMAGE_EDGE_ABANDON;\
                goto LOOP_Y_IN_IMAGE_##image##x##y##stride_x##stride_y##op;\
                break;\
            case IMAGE_EDGE_ABANDON:\
                break;\
            }\
        }

#define setPixelWhenTraversalPixelsOnThisPostion(dest_img,r,g,b,a)\
        {\
            dest_img.getPixel(absolute_x,absolute_y).setRGBA(r,g,b,a);\
        }

#endif // ANDROMEDA_MEDIA_IMAGEPROCESS
