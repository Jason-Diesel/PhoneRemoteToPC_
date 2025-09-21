using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using UnityEngine.UI;

public enum CurrentScene
{
    BUTTONS,
    LPCODE
}

public struct SaveData
{
    public string backgroundPath;
    public float dimmerSetting;
    public float timeUntilFade;

    private float _FadeTime;
    public float fadeTime
    {
        get { return _FadeTime; }
        set
        {
            if(value <= 0)
            {
                Debug.Log("why");
            }
            _FadeTime = value;
        }
    }
    
}

public class Settings : MonoBehaviour
{
    public GameObject _settings;
    public GameObject _buttons;
    public GameObject _lpCode;

    public GameObject _settingsButton;

    private Coroutine saveCoroutine;

    public BackImagesHandler backImageHandler;
    public HideUI hideUI;

    public Slider DimmerSlider;
    public Slider timeUntilFadeSlider;
    public Slider fadeTimeSlider;

    private string savedSaveFilePath;
    SaveData _saveData;

    CurrentScene _currentScene = CurrentScene.LPCODE;

    public void Start()
    {
        savedSaveFilePath = Path.Combine(Application.persistentDataPath, "saveDatas.data");
        if(File.Exists(savedSaveFilePath))
        {
            using (BinaryReader reader = new BinaryReader(File.Open(savedSaveFilePath, FileMode.Open)))
            {
                _saveData.backgroundPath = reader.ReadString();
                _saveData.dimmerSetting = reader.ReadSingle();
                _saveData.timeUntilFade = reader.ReadSingle();
                _saveData.fadeTime = reader.ReadSingle();
            }
        }
        else
        {
            _saveData.backgroundPath = "";
            _saveData.dimmerSetting = 1.0f;
            _saveData.timeUntilFade = 10.0f;
            _saveData.fadeTime = 2.0f;
        }
        
        //SET SETTINGS SHIT
        {
            if (_saveData.backgroundPath != "")
            {
                backImageHandler.pickImageOrVideo(_saveData.backgroundPath);
            }

            hideUI.setUp(_saveData);
            DimmerSlider.value = _saveData.dimmerSetting;
            timeUntilFadeSlider.SetValueWithoutNotify(_saveData.timeUntilFade);
            fadeTimeSlider.SetValueWithoutNotify(_saveData.fadeTime);
        }
    }

    public void saveData()
    {
        using (BinaryWriter writer = new BinaryWriter(File.Open(savedSaveFilePath, FileMode.Create)))
        {
            writer.Write(_saveData.backgroundPath ?? "");
            writer.Write(_saveData.dimmerSetting);
            writer.Write(_saveData.timeUntilFade);
            writer.Write(_saveData.fadeTime);
        }
        Debug.Log("Save Data:" + _saveData.fadeTime);
    }

    private IEnumerator SaveAfterDelay(float delay)
    {
        yield return new WaitForSeconds(delay);
        saveData();
        saveCoroutine = null;
    }

    public void saveFade(float TimeUntilStartingToFade, float TimeUntilFinishedFade)
    {
        _saveData.timeUntilFade = TimeUntilStartingToFade;
        _saveData.fadeTime = TimeUntilFinishedFade;

        if (saveCoroutine != null)
            StopCoroutine(saveCoroutine);

        saveCoroutine = StartCoroutine(SaveAfterDelay(4f));
    }

    public void saveDimmer(float dimmer)
    {
        _saveData.dimmerSetting = dimmer;

        if (saveCoroutine != null)
            StopCoroutine(saveCoroutine);

        saveCoroutine = StartCoroutine(SaveAfterDelay(4f));
    }

    public void saveBackground(string background)
    {
        _saveData.backgroundPath = background;
        saveData();
    }

    public void Back()
    {
        switch (_currentScene)
        {
            case CurrentScene.BUTTONS:
                ToButtonScreen();
                break;
            case CurrentScene.LPCODE:
                ToconnectScreen();
                break;
        }
    }
    public void ToconnectScreen()
    {
        _settingsButton.SetActive(true);
        _currentScene = CurrentScene.LPCODE;
        _settings.SetActive(false);
        _buttons.SetActive(false);
        _lpCode.SetActive(true);
    }
    public void ToButtonScreen()
    {
        _settingsButton.SetActive(true);
        _currentScene = CurrentScene.BUTTONS;
        _settings.SetActive(false);
        _buttons.SetActive(true);
        _lpCode.SetActive(false);
    }
    public void ToSettings()
    {
        _settingsButton.SetActive(false);
        _settings.SetActive(true);
        _buttons.SetActive(false);
        _lpCode.SetActive(false);
    }

    public CurrentScene getCurrentScene()
    {
        return _currentScene;
    }
}
