//
// Created by screemer on 2/11/19.
//

#include "ps_zoom_btn.h"

void PsZoomBtn::update(long time) {
    long timeSinceLastFrame = time - lastTime;

    if (up) {
        zoom += (zoomStep * timeSinceLastFrame);
        w = ow * zoom;
        h = oh * zoom;
    } else {
        zoom -= (zoomStep * timeSinceLastFrame);
        w = ow * zoom;
        h = oh * zoom;
    }

    if (zoom >= maxZoom) {
        up = false;
        zoom = maxZoom;
    }
    if (zoom <= 1.0f) {
        up = true;
        zoom = 1.0f;
    }


    int sizeChangeX = w - ow;
    int sizeChangeY = h - oh;

    x = ox - (sizeChangeX / 2);
    y = oy - (sizeChangeY / 2);

    lastTime = time;
}
