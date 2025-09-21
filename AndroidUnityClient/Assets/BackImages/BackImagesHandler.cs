using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Video;
using NativeGalleryNamespace;
using System.IO;

public class BackImagesHandler : MonoBehaviour
{
    public Image image;
    public VideoPlayer videoPlayer;

    public RawImage rawImage;
    public Slider slider;
    public Settings _settings;

    public BackgroundScaler backgroundScaler;

    private void Awake()
    {
        videoPlayer.gameObject.SetActive(false);
        image.gameObject.SetActive(false);
    }
    public void PickImageOrVideo()
    {
        if (NativeGallery.CanSelectMultipleMediaTypesFromGallery())
        {
            if (NativeGallery.IsMediaPickerBusy())
                return;
            NativeGallery.GetMixedMediaFromGallery((path) =>
            {
                Debug.Log("Media path: " + path);
                if (path != null)
                {
                    _settings.saveBackground(path);
                    // Determine if user has picked an image, video or neither of these
                    switch (NativeGallery.GetMediaTypeOfFile(path))
                    {
                        case NativeGallery.MediaType.Image: 
                            Debug.Log("Picked image");
                            PickImage(path);
                            break;
                        case NativeGallery.MediaType.Video:
                            Debug.Log("Picked video");
                            PickVideo(path);
                            break;
                        default: 
                            Debug.Log("Probably picked something else"); 
                            break;
                    }
                    backgroundScaler.setBackgroundAspectRatio();
                }
            }, NativeGallery.MediaType.Image | NativeGallery.MediaType.Video, "Select an image or video");
        }
    }

    public void pickImageOrVideo(string path)
    {
        NativeGallery.MediaType mediaType = NativeGallery.GetMediaTypeOfFile(path);
        switch(mediaType)
        {
            case NativeGallery.MediaType.Image:
                PickImage(path);
                break;
            case NativeGallery.MediaType.Video:
                PickVideo(path);
                break;
            default:
                break;
        }
    }

    private void PickImage(string path)
    {
        if (path != null)
        {
            image.gameObject.SetActive(true);
            videoPlayer.gameObject.SetActive(false);
            rawImage.gameObject.SetActive(false);

            // Load image from file as Texture2D
            Texture2D texture = NativeGallery.LoadImageAtPath(path, 1);
            if (texture != null)
            {
                Sprite sprite = Sprite.Create(texture,
                    new Rect(0, 0, texture.width, texture.height),
                    new Vector2(0.5f, 0.5f)
                    );
                image.sprite = sprite;
                image.color = new Color(255,255,255,255);
            }
        }
    }

    private void PickVideo(string path)
    {
        if (path != null)
        {
            image.gameObject.SetActive(false);
            videoPlayer.gameObject.SetActive(true);
            rawImage.gameObject.SetActive(true);

            videoPlayer.source = VideoSource.Url;
            videoPlayer.url = path;
            videoPlayer.Prepare();

            videoPlayer.prepareCompleted += (vp) => vp.Play();
        }
    }

    public void DimmerChange()
    {
        //And the image
        float dimmer = slider.value;
        rawImage.color = new Color(
            dimmer,
            dimmer,
            dimmer,
            1
            );
        image.color = new Color(
            dimmer,
            dimmer,
            dimmer,
            1
            );
        _settings.saveDimmer(dimmer);
    }
}
