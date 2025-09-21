using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Reflection;
using System.Text;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public enum NetworkCall
{
    QUIT = -1,
    SECKEY = 0,
    MESSAGE = 1,
    SETUP = 2,
    CHECKDISCONNECT = 3,
    TEST = 4,
    ACTIONS = 1000,
    //Action2 = 1001, Action3 = 1002 and so on 
}

public enum ClientConnection
{
    Disconnected,
    TryingToConnect,
    Connected,
    JustDisconnected
}

public class NetworkHandler : MonoBehaviour
{
    public GameObject ActionButtonPrefab;
    public GameObject ButtonContentGroup;
    public GameObject ButtonUI;
    public GameObject ErrorText;
    public TMP_InputField inputField;
    public GameObject connectScreen;
    public Settings _settings;


    TcpClient tcpClient;
    const int BUFFERSIZE = 2048;
    byte[] buf = new byte[BUFFERSIZE];

    private string ipAddress = "";//Need to be changed later
    private const string normalIP = "192.168.1.1";
    private const int startPort = 49152;
    private int port = startPort;
    private ClientConnection clientConnection;

    private NetworkStream stream;
    private int nrOfButtons;
    private int lastNrOfButtons;

    private List<string> buttonsNames;
    

    private string savedIpFilePath;

    bool disconnectedFromServer = false;

    // Start is called before the first frame update
    void Start()
    {
        buttonsNames = new List<string>();
        ButtonUI.SetActive(false);
        lastNrOfButtons = 0;
        nrOfButtons = 0;

        tcpClient = new TcpClient();

        savedIpFilePath = Path.Combine(Application.persistentDataPath, "IpCode.txt");
        Debug.Log(savedIpFilePath);

        if (File.Exists(savedIpFilePath))
        {
            string[] IpAndPort = File.ReadAllText(savedIpFilePath).Split(':');
            ipAddress = IpAndPort[0];
            port = Int32.Parse(IpAndPort[1]);
        }
        if(ipAddress != "")
        {
            if(ConnectedToServer())
            {
                _settings.ToButtonScreen();
            }
        }
        

    }

    private void HandleDisconnect()
    {
        Debug.Log("Disconnected");
        tcpClient.Close();
        tcpClient = new TcpClient();
        _settings.ToconnectScreen();
    }

    private void MessageRecv(IAsyncResult res)
    {
        try
        {
            Debug.Log("trying to recv message");
            if (!tcpClient.Connected)
            {
                disconnectedFromServer = true;
                return;
            }
            else if (res.IsCompleted)
            {
                int bytesIn = stream.EndRead(res);
                if(bytesIn == 0)
                {
                    disconnectedFromServer = true;
                    return;
                }

                

                //process message
                int pointerIndex = 0;
                int sizeofPackage = 0;
                int totalSize = 0;

                while(totalSize < bytesIn)
                {
                    //read size of package
                    sizeofPackage = BitConverter.ToInt32(buf, pointerIndex);
                    totalSize += sizeofPackage;
                    pointerIndex += sizeof(Int32);

                    int serverInstruction = BitConverter.ToInt32(buf, pointerIndex);
                    pointerIndex += sizeof(Int32);

                    Debug.Log("Server instruction is");
                    Debug.Log(serverInstruction);

                    switch (serverInstruction)
                    {
                        case (int)NetworkCall.SETUP:
                            setUpButtons(buf, ref pointerIndex);
                            break;

                    }
                }
                
                
                
            }
            Array.Clear(buf, 0, buf.Length);
            stream.BeginRead(buf, 0, buf.Length, MessageRecv, null);
        }
        catch (Exception ex)
        {
            Debug.LogWarning($"Lost connection: {ex.Message}");
            disconnectedFromServer = true;
        }

    }

    private void setUpButtons(byte[] buf, ref int pointerIndex)
    {
        nrOfButtons = BitConverter.ToInt32(buf, pointerIndex);
        pointerIndex += sizeof(Int32);

        buttonsNames.Clear();

        for(int i = 0; i < nrOfButtons; i++)
        {
            int stringSize = BitConverter.ToInt32(buf, pointerIndex);
            pointerIndex += sizeof(Int32);
            string buttonName = Encoding.ASCII.GetString(buf, pointerIndex, stringSize);
            pointerIndex += stringSize;

            buttonsNames.Add(buttonName);
        }
    }

    private void FixedUpdate()
    {
        if(lastNrOfButtons != nrOfButtons)
        {
            lastNrOfButtons = nrOfButtons;
            CreateButtons(nrOfButtons);
        }
        if(disconnectedFromServer)
        {
            disconnectedFromServer = false;
            HandleDisconnect();
        }
    }

    public void SendAction(int action)
    {
        //Send action to server
        byte[] msg = BitConverter.GetBytes(action);
        stream.Write(msg, 0, msg.Length);
    }

    private void OnDestroy()
    {
        if (tcpClient.Connected)
        {
            tcpClient.Close();
        }
    }

    private void CreateButtons(int nrOfButtons)
    {
        for(int i = 0; i < nrOfButtons; i++)
        {
            GameObject T = Instantiate(ActionButtonPrefab);
            T.GetComponent<Button>().onClick.RemoveAllListeners();

            int actionIndex = ((int)NetworkCall.ACTIONS) + i + 1; // capture this value
            T.GetComponent<Button>().onClick.AddListener(() => SendAction(actionIndex));

            T.GetComponentInChildren<TextMeshProUGUI>().text = buttonsNames[i];
            
            T.transform.SetParent(ButtonContentGroup.transform); 
        }
    }

    public void setIp()
    {
        string IpCode = inputField.text;
        string[] codes = IpCode.Split('.');
        string[] baseIp = normalIP.Split('.');

        int start = baseIp.Length - (codes.Length - 1);
        for (int i = 0; i < codes.Length - 1; i++)
        {
            baseIp[start + i] = codes[i];
        }

        port = startPort + Int32.Parse(codes[codes.Length - 1]);

        this.ipAddress = string.Join(".", baseIp);

        //save the IpCode for next time
        File.WriteAllText(savedIpFilePath, this.ipAddress + ":" + port.ToString());
        if (ConnectedToServer())
        {
            //REMOVE CONNECT
            //connectScreen.SetActive(false);
            //ButtonUI.SetActive(true);
            _settings.ToButtonScreen();
        }
        else
        {
            //ERROR MESSAGE
            Debug.Log("cannot connect");
            ErrorText.GetComponent<TMP_Text>().text += "cannot connect to server\n";
        }
    }

    private bool ConnectedToServer()
    {
        ErrorText.GetComponent<TMP_Text>().text = "";
        try
        {
            tcpClient.ConnectAsync(ipAddress, port).Wait(1000);//wait max 1 seconds
            if (!tcpClient.Connected)
            {
                ErrorText.GetComponent<TMP_Text>().text += "Cannot connect to server\n";
                Debug.Log("Error client not connected");
                return false;
            }
            stream = tcpClient.GetStream();
            stream.BeginRead(buf, 0, buf.Length, MessageRecv, null);
            return true;
        }
        catch (SocketException ex)
        {
            ErrorText.GetComponent<TMP_Text>().text += $"Socket error: {ex.Message}\n";
            Debug.LogWarning($"Socket error: {ex.Message}");
            return false;
        }
        catch (Exception ex)
        {
            ErrorText.GetComponent<TMP_Text>().text += $"Unexpected error: {ex.Message}\n";
            Debug.LogError($"Unexpected error: {ex.Message}");
            return false;
        }
    }

    public void Disconnect()
    {
        if (tcpClient.Connected)
        {
            HandleDisconnect();
        }
    }
}
