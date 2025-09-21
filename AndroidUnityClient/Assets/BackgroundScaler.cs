using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Video;

public class BackgroundScaler : MonoBehaviour
{
    public RawImage rawImage;
    public VideoPlayer videoPlayer;
    public AspectRatioFitter aspectFitter;

    public void setBackgroundAspectRatio()
    {
        if (videoPlayer.texture != null)
        {
            float w = videoPlayer.texture.width;
            float h = videoPlayer.texture.height;
            aspectFitter.aspectRatio = w / h;
        }
    }
}
