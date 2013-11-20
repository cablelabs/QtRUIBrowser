curl --header "content-type: application/soap+xml" -H "SOAPAction: urn:schemas-upnp-org:service:RemoteUIServer:1#GetCompatibleUIs" --data '<?xml version="1.0" encoding="utf-8"?>
<s:Envelope s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/" xmlns:s="http://schemas.xmlsoap.org/soap/envelope/">
 <s:Body>
  <u:GetCompatibleUIs xmlns:u="urn:schemas-upnp-org:service:RemoteUIServer:1">
   <InputDeviceProfile/>
   <UIFilter>*</UIFilter>
  </u:GetCompatibleUIs>
 </s:Body>
</s:Envelope>' $1
