#include <tiffio.h>

/* file:/usr/share/doc/libtiff-devel-3.5.7/html/libtiff.html#Tiles */

int
test_tiff(void)
{
    auto *tif = TIFFOpen("myfile.tif", "r");
    if (not tif) {
        perror("TIFFOpen");
        return -1;
    }

    uint32 imgWidth, imgLength;
    uint32 tileWidth, tileLength;
    uint32 x, y;
    tdata_t buf;

    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imgWidth);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imgLength);
    TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tileWidth);
    TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileLength);

    buf = _TIFFmalloc(TIFFTileSize(tif));

    for (y = 0; y < imgLength; y += tileLength) {
        for (x = 0; x < imgWidth; x += tileWidth) {
            /* \todo PROcess here... */
/*             TIFFReadTile(tif, buf, x, y, 0); */
        }
    }
    _TIFFfree(buf);
    TIFFClose(tif);

    return 0;
}

int
main(void)
{
    test_tiff();
    return 0;
}
