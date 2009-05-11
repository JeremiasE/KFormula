"""
Python invert script.

The python invert script inverts all pixels at the activate layer in
the current image.

Copyright (c) 2005-2006 Cyrille Berger <cberger@cberger.net>
Published under the GNU GPL =v2
"""

class Inverter:
    def __init__(self):

        # import the Krita module.
        try:
            import Krita
        except:
            raise "Import of the Krita module failed."

        # fetch the image.
        image = Krita.image()

        # we like to manipulate the active painting layer.
        layer = Krita.activeLayer().paintDevice()

        # get the height and the width the layer has.
        width = layer.width()
        height = layer.height()

        # we like to use the progressbar
        shell = Krita.shell()
        shell.slotSetStatusBarText("invert.py")
        shell.slotProgress(0)
        size = width * height
        progress = 0
        pixeldone = 0
        try:

            # tell Krita that painting starts. the whole painting session will be
            # counted till layer.endPainting() was called as one undo/redo-step.
            layer.beginPainting("invert")

            # create an iterator to walk over all pixels the layer has.
            it = layer.createRectIterator( 0, 0, width, height )

            # iterate over all pixels and invert each pixel.
            print "kikoo\n"
            finesh = it.isDone()
            while (not finesh):
                #p = it.channel()
                #p[0] = 255 - p[0]
                #p[1] = 255 - p[1]
                #p[2] = 255 - p[2]
                #it.setCannel(p)

                # invert the color of the pixel.
                it.invertColor()

                # increment the progress to show, that work on this pixel is done.
                percent = pixeldone * 100 / size
                if percent != progress:
                    progress = percent
                    shell.slotProgress( progress )
                pixeldone += 1

                # go to the next pixel.
                finesh = it.next()

            # painting is done now.
            layer.endPainting()

        finally:
            # finish progressbar
            shell.slotProgress(-1)
            shell.slotSetStatusBarText("")

Inverter()
