using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class CustomFieldInput : MonoBehaviour
{
    public TMP_InputField inputField;
    void Awake()
    {
        inputField.characterValidation = TMP_InputField.CharacterValidation.None;
    }
}
