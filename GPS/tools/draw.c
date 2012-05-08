#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TST_PRINT  0

#define SUCCESS_EC 0
#define UNSUCCESS_EC -1

typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef int int32_t;

typedef struct  {
  unsigned char magic[2];
} bmpfile_magic__t;

typedef struct  {
  uint32_t filesz;
  uint16_t creator1;
  uint16_t creator2;
  uint32_t bmp_offset;
} bmpfile_header__t;

typedef struct {
  uint32_t header_sz;
  int32_t width;
  int32_t height;
  uint16_t nplanes;
  uint16_t bitspp;
  uint32_t compress_type;
  uint32_t bmp_bytesz;
  int32_t hres;
  int32_t vres;
  uint32_t ncolors;
  uint32_t nimpcolors;
} bitmap_info_header__t;

typedef enum {
  BI_RGB = 0,
  BI_RLE8,
  BI_RLE4,
  BI_BITFIELDS, //Also Huffman 1D compression for BITMAPCOREHEADER2
  BI_JPEG,      //Also RLE-24 compression for BITMAPCOREHEADER2
  BI_PNG,
} bmp_compression_method_t;


unsigned char get_bit__r(unsigned char c, int bit_idx)
{
	return ((c>>bit_idx)&1);
}

void set_bit__r( unsigned char *c, int bit_idx)
{
	*c = *c | (1<<bit_idx);
}

void lcd2array_mapping__r(int lcd_byte, int lcd_bit, int lcd_width, int *col, int *row)
{
	*col = lcd_byte%lcd_width;
	*row = ((int)(lcd_byte/lcd_width))*8+(7-lcd_bit);
}

void array2lcd_mapping__r(int row, int col, int lcd_width, int *lcd_byte, int *lcd_bit)
{
	*lcd_bit = 7 - (row%8);
	*lcd_byte = ((int)(row/8))*lcd_width+col;
}

void bmp2array_mapping__r(int bmp_byte, int bmp_bit, int bmp_height, int bmp_width, int *col, int *row)
{
	*row = bmp_height-1-(int)(bmp_byte*8/bmp_width);
	*col = (bmp_byte*8+7-bmp_bit)%bmp_width;
}

void array2bmp_mapping__r(int col, int row, int bmp_height, int bmp_width, int *bmp_byte, int *bmp_bit)
{
	int point_idx;
	
	point_idx = (bmp_height-1-row)*bmp_width + col;
	*bmp_byte = (int)(point_idx/8);
	*bmp_bit = 7 - (point_idx%8);
	
}

void print_guide__r()
{
  printf("\n");
  printf("/***************************************************************/\n");
  printf("/* B2LConverter:                                               */\n");
  printf("/* --used to convert BMP picture to LCD display byte stream;   */\n");
  printf("/* --used to convert LCD display byte stream to BMP picture;   */\n");
  printf("/*-------------------------------------------------------------*/\n");
  printf("/* Usage:                                                      */\n");
  printf("/*  BMP => LCD: B2LConverter b2l [BMP file name]               */\n");
  printf("/*          eg: B2LConverter b2l test.bmp                      */\n");
  printf("/*  LCD => BMP: B2LConverter l2b <height> <width> [TXT file]   */\n");
  printf("/*          eg: B2LConverter l2b 64 128 test.txt               */\n");
  printf("/***************************************************************/\n");
  printf("\n");
}

int read_bmp_file_to_buffer__r( const char *p_file_name, 
                                char **p_data_buff, 
                                uint32_t *p_data_size,
                                uint32_t *p_width,
                                uint32_t *p_height )
{
  int ret_val = SUCCESS_EC;
  long file_size;
  FILE *ptr_file = NULL;
  char *bmp_file_buff = NULL;
  char *bmp_data_buff = NULL;
  bmpfile_header__t *ptr_bmpfile_hd = NULL;
  bitmap_info_header__t *ptr_bmp_info_hd = NULL;
  
#if TST_PRINT
  printf("==> read_bmp_file_to_buffer__r %s\n", p_file_name);
#endif
  
  ptr_file = fopen(p_file_name, "rb");
  if(NULL==ptr_file)
  {
    printf("/*** Fail to open file %s! ***/\n", p_file_name);
    return UNSUCCESS_EC;
  }
  
  fseek (ptr_file , 0 , SEEK_END);
  file_size = ftell (ptr_file);
  rewind (ptr_file);
  
  bmp_file_buff = (char *)malloc(file_size);
  if(bmp_file_buff == NULL)
  {
    printf("/*** Fail to allocate memery buffer for BMP file! ***/\n");
    fclose(ptr_file);
    return UNSUCCESS_EC;
  }

  fread(bmp_file_buff, 1, file_size, ptr_file);
  fclose(ptr_file);
	
  ptr_bmpfile_hd = (bmpfile_header__t *)(bmp_file_buff + sizeof(bmpfile_magic__t));
  ptr_bmp_info_hd = (bitmap_info_header__t *)( bmp_file_buff + 
                                               sizeof(bmpfile_magic__t) +
                                               sizeof(bmpfile_header__t) );
  *p_data_size = ptr_bmp_info_hd->bmp_bytesz;
  *p_width = ptr_bmp_info_hd->width;
  *p_height = ptr_bmp_info_hd->height;

#if TST_PRINT
  printf("p_data_size: %d\n", *p_data_size);
  printf("p_width:     %d\n", *p_width);
  printf("p_height:    %d\n", *p_height);
#endif
  
  if( (*p_width%8 != 0) || (*p_height%8 != 0) )
  {
    printf("/*** The size of BMP file is not (8M)X(8N)! ***/\n");
    free(bmp_file_buff);
    return UNSUCCESS_EC;
  }
  
  *p_data_buff = (char *)malloc(*p_data_size);
  if( *p_data_buff == NULL )
  {
    printf("/*** Fail to allocate memery buffer for BMP data! ***/\n");
    free(bmp_file_buff);
    return UNSUCCESS_EC;
  }
  
  memcpy( *p_data_buff, 
          (bmp_file_buff+ptr_bmpfile_hd->bmp_offset ),
          *p_data_size );
  free(bmp_file_buff);
    
  return SUCCESS_EC;
}

void map_bmp_to_bit_array__r( char *p_bmp_data_buff,
                              char *p_bit_array,
                              uint32_t p_bmp_data_size,
                              uint32_t p_bmp_width,
                              uint32_t p_bmp_height )
{
  int i, j, col, row;
  
#if TST_PRINT
  printf("==> map_bmp_to_bit_array__r\n");
#endif  
  
  for(i=0; i<p_bmp_data_size; i++)
  {
    for(j=0;j<8;j++)
    {
      bmp2array_mapping__r(i, j, p_bmp_height, p_bmp_width, &col, &row);
      if(get_bit__r(*(p_bmp_data_buff+i), j)>0)
      {
        *(p_bit_array + row*p_bmp_width + col)=1;
      }
    }
  }
}

void map_bit_array_to_lcd_stream__r( char *p_bit_array,
                                     unsigned char *p_lcd_stream,
                                     uint32_t p_bmp_width,
                                     uint32_t p_bmp_height )
{
  int i, j, col, row;
  
#if TST_PRINT
  printf("==> map_bit_array_to_lcd_stream__r\n");
#endif    
  
  for(row=0; row<p_bmp_height; row++)
  {
    for(col=0;col<p_bmp_width;col++)
    {
      array2lcd_mapping__r(row, col, p_bmp_width, &i, &j);
      if(*(p_bit_array + row*p_bmp_width + col) > 0)
      {
        set_bit__r((p_lcd_stream+i), j);
#if TST_PRINT
        printf("*");
#endif
      }
      else
      {
#if TST_PRINT
        printf(" ");
#endif
      }			
    }
#if TST_PRINT
    printf("\n");
#endif
  }  
}

int write_lcd_stream_to_file__r( unsigned char *p_lcd_stream,
                                 uint32_t p_lcd_stream_len,
                                 const char *p_file_name )
{
  FILE *ptr_file = NULL;
  int i;

#if TST_PRINT
  printf("==> write_lcd_stream_to_file__r\n");
#endif    
  
  ptr_file = fopen(p_file_name, "wt");
  if( ptr_file == NULL )
  {
    printf("/*** Fail to create LCD stream file %s! ***/\n", p_file_name);
    return UNSUCCESS_EC;
  }
  
  for(i=0; i<p_lcd_stream_len;i++)
  {
    fprintf(ptr_file, "0x%02X,", *(p_lcd_stream+i));
    
    if(i%16 == 15)
      fprintf(ptr_file, "\n");   
  }
  
  fclose(ptr_file);
  
  return SUCCESS_EC;
}

int convert_bmp_to_lcd__r( const char *p_file_name )
{
  int ret_val = SUCCESS_EC;
  char *ptr_bmp_data_buff = NULL;
  char *ptr_bit_array = NULL;
  unsigned char *ptr_lcd_stream = NULL;
  uint32_t bmp_data_size, bmp_width, bmp_height;
  
#if TST_PRINT
  printf("==> converst bmp to lcd from %s\n", p_file_name);
#endif
  
  ret_val = read_bmp_file_to_buffer__r( p_file_name,
                                        &ptr_bmp_data_buff,
                                        &bmp_data_size,
                                        &bmp_width,
                                        &bmp_height );
  if( ret_val != SUCCESS_EC )
  {
    return ret_val;
  }
  
  ptr_bit_array = (char *)malloc(bmp_width*bmp_height);
  if( ptr_bit_array == NULL )
  {
    printf("/*** Fail to allocate memery for bit array! ***/\n");
    free(ptr_bmp_data_buff);
    return UNSUCCESS_EC;
  }
  memset(ptr_bit_array, 0, bmp_width*bmp_height);
  
  map_bmp_to_bit_array__r( ptr_bmp_data_buff, 
                           ptr_bit_array,
                           bmp_data_size,
                           bmp_width,
                           bmp_height );  
  free(ptr_bmp_data_buff);
  
  ptr_lcd_stream = (unsigned char *)malloc(bmp_width*bmp_height/8);
  if( ptr_lcd_stream == NULL )
  {
    printf("/*** Fail to allocate memery for LCD steam! ***/\n");
    free(ptr_bit_array);
    return UNSUCCESS_EC;
  }
  memset( ptr_lcd_stream, 0, bmp_width*bmp_height/8 );
  
  map_bit_array_to_lcd_stream__r( ptr_bit_array,
                                  ptr_lcd_stream,
                                  bmp_width,
                                  bmp_height );                                                                   
  free(ptr_bit_array);  
  
  write_lcd_stream_to_file__r( ptr_lcd_stream, bmp_width*bmp_height/8, "output.txt" );
  free(ptr_lcd_stream);
  
  return SUCCESS_EC;
}

unsigned int hex_str_to_int( char *p_str )
{
    unsigned int ret = 0;
    
    if( !((*p_str=='0') && (*(p_str+1)=='x' || *(p_str+1)=='X')) )
        return ret;
    
    p_str += 2;
    while((*p_str<='9' && *p_str>='0') || (*p_str<='F' && *p_str>='A') || 
          (*p_str<='f' && *p_str>='a'))
    {
        if(*p_str>='0' && *p_str<='9')
        {
            ret = ret*16 + *p_str - '0';
        }
        else if(*p_str<='F' && *p_str>='A')
        {
            ret = ret*16 + 10 + *p_str - 'A';
        }
        else if(*p_str<='f' && *p_str>='a')
        {
            ret = ret*16 + 10 + *p_str - 'a';
        }
        
        p_str++;
    }
    
    return ret;
}

int read_lcd_stream_to_buffer__r( const char *p_file_name,
                                  uint32_t p_lcd_str_len,
                                  unsigned char **p_lcd_buff )
{
  int  ret_val = SUCCESS_EC;
  FILE *ptr_file = NULL;
  long file_size;
  char *file_buff = NULL;
  char *ptr_read_hdr = NULL;
  char *ptr_read_tear = NULL;
  int  lcd_buff_idx; 
  
#if TST_PRINT
  printf("==> read_lcd_stream_to_buffer__r\n");
#endif
  
  ptr_file = fopen(p_file_name, "rt");
  if(NULL==ptr_file)
  {
    printf("/*** Fail to open file %s! ***/\n", p_file_name);
    return UNSUCCESS_EC;
  }
  
  fseek (ptr_file , 0 , SEEK_END);
  file_size = ftell (ptr_file);
  rewind (ptr_file);
  
  file_buff = (char *)malloc(file_size);
  if( file_buff == NULL )
  {
    printf("/*** Fail to allocate memery for LCD stream file! ***/\n");
    fclose(ptr_file);
    return UNSUCCESS_EC;
  }
  
  fread(file_buff, 1, file_size, ptr_file);
  fclose(ptr_file);
  
  *p_lcd_buff = (unsigned char *)malloc(p_lcd_str_len);
  if( *p_lcd_buff == NULL )
  {
    printf("/*** Fail to allocate memery for LCD stream buffer! ***/\n");
    free(file_buff);
    return UNSUCCESS_EC;
  }
  
  ptr_read_hdr = file_buff;
  
  for( lcd_buff_idx=0; lcd_buff_idx<p_lcd_str_len; lcd_buff_idx++ )
  {
    while( (ptr_read_hdr < file_buff+file_size) &&
           !((*ptr_read_hdr<='9' && *ptr_read_hdr>='0') || (*ptr_read_hdr<='F' && *ptr_read_hdr>='A') || 
           (*ptr_read_hdr<='f' && *ptr_read_hdr>='a') || (*ptr_read_hdr=='x') || (*ptr_read_hdr=='X')) )
    {
        ptr_read_hdr++;
    }
    
    if(ptr_read_hdr - file_buff >= file_size )
    {
      printf("/*** Not enough LCD stream for BMP file! ***/\n");
      free(file_buff);
      free(*p_lcd_buff);
      return UNSUCCESS_EC;
    }
    
    for( ptr_read_tear=ptr_read_hdr; ptr_read_tear<(file_buff+file_size); ptr_read_tear++ )
    {
        if(!((*ptr_read_tear<='9' && *ptr_read_tear>='0') || (*ptr_read_tear<='F' && *ptr_read_tear>='A') || 
           (*ptr_read_tear<='f' && *ptr_read_tear>='a') || (*ptr_read_tear=='x') || (*ptr_read_tear=='X')) )
        {
            break;
        }              
    }
    
    if(ptr_read_tear - file_buff >= file_size )
    {
      printf("/*** Not enough LCD stream for BMP file! ***/\n");
      free(file_buff);
      free(*p_lcd_buff);
      return UNSUCCESS_EC;
    }
    
    *ptr_read_tear = '\0';   
    *(*p_lcd_buff+lcd_buff_idx) = (unsigned char)hex_str_to_int(ptr_read_hdr);
    ptr_read_hdr = ptr_read_tear + 1;    
  }
  
  free(file_buff);
  
  return SUCCESS_EC;
}

void map_lcd_stream_to_bit_array__r( unsigned char *p_lcd_stream,
                                     uint32_t p_lcd_str_len,
                                     uint32_t p_width,
                                     uint32_t p_height,
                                     char *p_bit_array )
{
    int  i, j, row, col;
    
#if TST_PRINT
    printf("==> map_lcd_stream_to_bit_array__r\n");
#endif
    
    for( i=0; i<p_lcd_str_len; i++ )
	{
		for(j=0;j<8;j++)
		{
			lcd2array_mapping__r(i, j, p_width, &col, &row);
			if(get_bit__r(*(p_lcd_stream+i), j)>0)
				*(p_bit_array + row*p_width + col) = 1;
		}
	}

#if TST_PRINT
    for(row=0; row<p_height; row++)
    {
        for(col=0; col<p_width; col++)
        {
            if(*(p_bit_array + row*p_width + col) > 0)
            {
                printf("*");
            }
            else
            {
                printf(" ");
            }
        }
        printf("\n");
    }
#endif    
    
}

void map_bit_array_to_bmp__r( char *p_bit_array,
                              uint32_t p_bmp_data_len,
                              uint32_t p_width,
                              uint32_t p_height,
                              unsigned char *p_bmp_data )
{
    int  i, j, row, col;
    
#if TST_PRINT
    printf("==> map_bit_array_to_bmp__r\n");
#endif
    
    for(row=0; row<p_height; row++)
	{
		for(col=0;col<p_width;col++)
		{			
			array2bmp_mapping__r(col, row, p_height, p_width, &i, &j);
			if(*(p_bit_array + row*p_width + col) > 0)
			{
				set_bit__r(p_bmp_data+i, j);
			}						
		}
	}
    
}

int write_bmp_file__r( unsigned char *p_bmp_data,
                       char *p_file_name,
                       uint32_t p_bmp_data_len,
                       uint32_t p_width,
                       uint32_t p_height )
{
    int ret_val = SUCCESS_EC;
    FILE *ptr_file = NULL;
    unsigned char *bmp_file_buff = NULL;
    int  bmp_hdr_len;
    bmpfile_header__t *ptr_bmp_file_hdr = NULL;
    bitmap_info_header__t *ptr_bmp_info_hdr = NULL;
    uint32_t *color_value = NULL;
    
#if TST_PRINT
    printf("==> write_bmp_file__r\n");
#endif

    bmp_hdr_len = sizeof(bmpfile_magic__t) + sizeof(bmpfile_header__t) +
                  sizeof(bitmap_info_header__t) + 2*sizeof(uint32_t);
    bmp_file_buff = (unsigned char *)malloc( bmp_hdr_len );
    if(bmp_file_buff == NULL)
    {
        printf("/*** Fail to allocate memery for BMP file header! ***/\n");
        return UNSUCCESS_EC;
    }
    memset(bmp_file_buff, 0, bmp_hdr_len);
    
    /* Magic Number */
    *bmp_file_buff = 'B';
    *(bmp_file_buff+1) = 'M';
    
    /* BMP file header */
    ptr_bmp_file_hdr = (bmpfile_header__t *)(bmp_file_buff+sizeof(bmpfile_magic__t));
	ptr_bmp_info_hdr = (bitmap_info_header__t *)(ptr_bmp_file_hdr+1);
	color_value = (uint32_t *)(ptr_bmp_info_hdr + 1);
	
	ptr_bmp_file_hdr->filesz = p_bmp_data_len + bmp_hdr_len;
    ptr_bmp_file_hdr->bmp_offset = bmp_hdr_len;
    
	ptr_bmp_info_hdr->header_sz = 40;
    ptr_bmp_info_hdr->width = p_width;
    ptr_bmp_info_hdr->height = p_height;
    ptr_bmp_info_hdr->nplanes = 1;
    ptr_bmp_info_hdr->bitspp = 1;
    ptr_bmp_info_hdr->compress_type = 0;
    ptr_bmp_info_hdr->bmp_bytesz = p_bmp_data_len;
	
	*color_value = 0;
	*(color_value+1) = 0xFFFFFF;
	
	ptr_file = fopen(p_file_name, "wb");
	if(NULL==ptr_file)
	{
		printf("/*** Fail to create BMP file %s! ***/!\n", p_file_name);
		free(bmp_file_buff);
		return UNSUCCESS_EC;
	}
	
	fwrite(bmp_file_buff, 1, bmp_hdr_len, ptr_file);
	fwrite(p_bmp_data, 1, p_bmp_data_len, ptr_file);
	
	free(bmp_file_buff);
	fclose(ptr_file);
	
    return SUCCESS_EC;
}


int convert_lcd_to_bmp__r( const char *p_file_name, 
                           uint32_t p_height,
                           uint32_t p_width )
{
  int  ret_val = SUCCESS_EC;
  unsigned char *lcd_buff = NULL;
  char *bit_array = NULL;
  unsigned char *bmp_data_buff = NULL;
  
#if TST_PRINT
  printf("==> convert_lcd_to_bmp__r\n");
  printf("    p_file_name: %s\n", p_file_name);
  printf("    p_height:    %d\n", p_height);
  printf("    p_width:     %d\n", p_width);
#endif
  
  ret_val = read_lcd_stream_to_buffer__r( p_file_name,
                                          p_height*p_width/8,
                                          &lcd_buff );
  if( ret_val != SUCCESS_EC )
  {
    return ret_val;
  }
  
  bit_array = (char *)malloc(p_height*p_width);
  if( bit_array == NULL )
  {
    free(lcd_buff);
    printf("/*** Fail to allocate memery for bit array! ***/\n");
    return UNSUCCESS_EC;
  }
  memset( bit_array, 0, p_height*p_width );
  
  map_lcd_stream_to_bit_array__r( lcd_buff, 
                                  p_height*p_width/8,
                                  p_width,
                                  p_height,
                                  bit_array );
  free(lcd_buff);
  
  bmp_data_buff = (unsigned char *)malloc(p_height*p_width/8);
  if( bmp_data_buff == NULL )
  {
    free(bit_array);
    printf("/*** Fail to allocate memery for BMP data buffer! ***/\n");
    return UNSUCCESS_EC;
  }
  memset(bmp_data_buff, 0, p_height*p_width/8);
  
  map_bit_array_to_bmp__r( bit_array,
                           p_height*p_width/8,
                           p_width,
                           p_height,
                           bmp_data_buff ); 
  free(bit_array);
  
  ret_val = write_bmp_file__r( bmp_data_buff,
                               "output.bmp",
                               p_height*p_width/8,
                               p_width,
                               p_height );
  free(bmp_data_buff); 
    
  return ret_val;
}



main(int argc, char *argv[])
{
  char *ptr = NULL;
  int  argv_len, tmp_len;
  uint32_t height, width;
  int  ret_val = SUCCESS_EC;
  
  if(2==argc && strcmp(argv[1],"b2l")==0 )
  {
    ret_val = convert_bmp_to_lcd__r("input.bmp");
  }
  else if(3==argc && strcmp(argv[1],"b2l")==0 )
  {
    argv_len = strlen(argv[2]);
    tmp_len = strlen(".bmp");
    if( (argv_len > tmp_len) && (strcmp(argv[2]+argv_len-tmp_len, ".bmp")==0) )
    {
      ret_val = convert_bmp_to_lcd__r(argv[2]);
    }
    else
    {
      printf("/*** Illegal file name: %s ***/\n", argv[2]);
      print_guide__r();
      return;
    }    
  }
  else if( (argc>=4 && argc<=5) && strcmp(argv[1], "l2b")==0 )
  {
    for( ptr=argv[2]; *ptr!='\0'; ptr++ )
    {
      if( *ptr<'0' || *ptr>'9' )
      {
        printf("/*** Illegal height value! ***/\n");
        print_guide__r();
        return;
      }
    }
    height = atoi(argv[2]);
    if( height%8 != 0 )
    {
      printf("/*** The height given is not 8N: %d! ***/\n", height);
      print_guide__r();
      return;
    }
    
    for( ptr=argv[3]; *ptr!='\0'; ptr++ )
    {
      if( *ptr<'0' || *ptr>'9' )
      {
        printf("/*** Illegal width value! ***/\n");
        print_guide__r();
        return;
      }
    }
    width = atoi(argv[3]);
    if( width%8 != 0 )
    {
      printf("/*** The width given is not 8N: %d! ***/\n", width);
      print_guide__r();
      return;
    }
    
    if( argc == 4 )
    {
      ret_val = convert_lcd_to_bmp__r("input.txt", height, width);
    }
    else
    {
      argv_len = strlen(argv[4]);
      tmp_len = strlen(".txt");
      if( (argv_len > tmp_len) && (strcmp(argv[4]+argv_len-tmp_len, ".txt")==0) )
      {
        ret_val = convert_lcd_to_bmp__r(argv[4], height, width);
      }
      else
      {
        printf("/*** Illegal file name: %s ***/\n", argv[4]);
        print_guide__r();
        return;
      }
    }    
  }
  else
  {
    print_guide__r();    
  }    
}

