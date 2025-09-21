using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class HideUI : MonoBehaviour
{
    public CanvasGroup everything;
    public GameObject ReactivateButton;
    public Settings settings;
    private float TimeUntilStartingToFade;
    private float TimeUntilFinishedFade;
    private float currentTimeToFade = 0;

    public Slider TimeUntilStartingToFadeSlider;
    public Slider TimeUntilFinishedFadeSlider;



    private void Awake()
    {
        //A
        TimeUntilStartingToFade = 5;
        TimeUntilFinishedFade = 2;
    }

    public void setUp(SaveData saveData)
    {
        TimeUntilStartingToFade = saveData.timeUntilFade;
        TimeUntilFinishedFade = saveData.fadeTime;
        Debug.Log("fade time" + TimeUntilFinishedFade);
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        if(settings.getCurrentScene() != CurrentScene.BUTTONS)
        { return; }

        currentTimeToFade += Time.fixedDeltaTime;

        if (currentTimeToFade < TimeUntilStartingToFade || TimeUntilStartingToFade <= 1)
        { return; }

        if (Input.GetMouseButtonDown(0) || Input.GetMouseButtonDown(1) || Input.GetMouseButtonDown(2))
        {
            removeFade();
            Debug.Log("remove fade with mouse");
        }
        if(currentTimeToFade > TimeUntilStartingToFade)
        {
            ReactivateButton.SetActive(true);
            startFade();
        }
    }
    private void startFade()
    {
        everything.alpha = 1 - (currentTimeToFade - TimeUntilStartingToFade) / TimeUntilFinishedFade;
    }
    public void removeFade()
    {
        currentTimeToFade = 0;
        everything.alpha = 1;
        ReactivateButton.SetActive(false);
    }

    public void ChangeFade()
    {
        currentTimeToFade = -2;
        TimeUntilStartingToFade = TimeUntilStartingToFadeSlider.value;
        TimeUntilFinishedFade = TimeUntilFinishedFadeSlider.value;
        settings.saveFade(TimeUntilStartingToFade, TimeUntilFinishedFade);
    }
}
