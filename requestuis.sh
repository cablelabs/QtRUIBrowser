curl --header "content-type: application/soap+xml" --data '<?xml version="1.0" encoding="utf-8"?>
<s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding">
 <s:Body>
  <u:GetCompatibleUIs xmlns:u="urn:schemas-upnp-org:service:RemoteUIServer:1">
   <InputDeviceProfile/>
   <UIFilter>*</UIFilter>
  </u:GetCompatibleUIs>
 </s:Body>
</s:Envelope>' $1
