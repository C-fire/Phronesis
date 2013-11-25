<?xml version='1.0' encoding='ISO-8859-1' standalone='yes' ?>
<tagfile>
  <compound kind="file">
    <name>phronesisAnalyzerClientApi.hpp</name>
    <path>/home/chaen/phronesis/src/analyzer/analyzerClientApi/</path>
    <filename>phronesisAnalyzerClientApi_8hpp</filename>
    <class kind="class">Observer</class>
    <class kind="class">Observable</class>
    <class kind="class">api_client</class>
    <class kind="class">AbstractPhronesisClient</class>
    <member kind="typedef">
      <type>boost::shared_ptr&lt; tcp_connection &gt;</type>
      <name>connection_ptr</name>
      <anchorfile>phronesisAnalyzerClientApi_8hpp.html</anchorfile>
      <anchor>a3081204dfd4813d0f99716a9627720cf</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>PhronesisConsole.cpp</name>
    <path>/home/chaen/phronesis/src/analyzer/analyzerClientApi/</path>
    <filename>PhronesisConsole_8cpp</filename>
  </compound>
  <compound kind="file">
    <name>PhronesisConsole.hpp</name>
    <path>/home/chaen/phronesis/src/analyzer/analyzerClientApi/</path>
    <filename>PhronesisConsole_8hpp</filename>
  </compound>
  <compound kind="file">
    <name>phrUtils.cpp</name>
    <path>/home/chaen/phronesis/src/analyzer/analyzerClientApi/</path>
    <filename>phrUtils_8cpp</filename>
    <includes id="phrUtils_8cpp" name="phrUtils.cpp" local="yes" imported="no">phrUtils.hpp</includes>
    <member kind="function">
      <type>int</type>
      <name>main</name>
      <anchorfile>phrUtils_8cpp.html</anchorfile>
      <anchor>a3c04138a5bfe5d72780bb7e82a18e627</anchor>
      <arglist>(int argc, char **argv)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>phrUtils.hpp</name>
    <path>/home/chaen/phronesis/src/analyzer/analyzerClientApi/</path>
    <filename>phrUtils_8hpp</filename>
    <includes id="phronesisAnalyzerClientApi_8hpp" name="phronesisAnalyzerClientApi.hpp" local="yes" imported="no">phronesisAnalyzerClientApi.hpp</includes>
    <includes id="PhronesisConsole_8hpp" name="PhronesisConsole.hpp" local="yes" imported="no">PhronesisConsole.hpp</includes>
    <class kind="class">coutConsole</class>
    <class kind="class">answerQuestion</class>
    <class kind="class">SetVetoInterface</class>
    <class kind="class">phrUtilsInterface</class>
    <member kind="enumvalue">
      <name>NONE</name>
      <anchorfile>phrUtils_8hpp.html</anchorfile>
      <anchor>a99fb83031ce9923c84392b4e92f956b5ac157bdf0b85a40d2619cbc8bc1ae5fe2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OBSERVE</name>
      <anchorfile>phrUtils_8hpp.html</anchorfile>
      <anchor>a99fb83031ce9923c84392b4e92f956b5aee714b9ff0fd3b4dfa63876da63e7981</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>QUESTION</name>
      <anchorfile>phrUtils_8hpp.html</anchorfile>
      <anchor>a99fb83031ce9923c84392b4e92f956b5ad126f509c35e1661274f8b72693c7848</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MESSAGE</name>
      <anchorfile>phrUtils_8hpp.html</anchorfile>
      <anchor>a99fb83031ce9923c84392b4e92f956b5ab3e6d7e18839259526a01001f558200f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SET_VETO</name>
      <anchorfile>phrUtils_8hpp.html</anchorfile>
      <anchor>a99fb83031ce9923c84392b4e92f956b5a6537b3144d78611d40b1ff6dcce67b55</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>GET_VETO</name>
      <anchorfile>phrUtils_8hpp.html</anchorfile>
      <anchor>a99fb83031ce9923c84392b4e92f956b5a1f3b455927b9ec6bc2734b6fb873b076</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>pyPhronesismodule.cpp</name>
    <path>/home/chaen/phronesis/src/analyzer/analyzerClientApi/</path>
    <filename>pyPhronesismodule_8cpp</filename>
    <includes id="phronesisAnalyzerClientApi_8hpp" name="phronesisAnalyzerClientApi.hpp" local="yes" imported="no">phronesisAnalyzerClientApi.hpp</includes>
    <class kind="class">ObserverWrap</class>
    <class kind="class">pyAbstractPhronesisClient</class>
    <member kind="function">
      <type></type>
      <name>BOOST_PYTHON_MODULE</name>
      <anchorfile>pyPhronesismodule_8cpp.html</anchorfile>
      <anchor>a4a10ecb51d4ff0002c9ba18957cb557f</anchor>
      <arglist>(pyPhronesis)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>setup.py</name>
    <path>/home/chaen/phronesis/src/analyzer/analyzerClientApi/</path>
    <filename>setup_8py</filename>
    <namespace>setup</namespace>
    <member kind="variable">
      <type>list</type>
      <name>ext_modules</name>
      <anchorfile>namespacesetup.html</anchorfile>
      <anchor>a75e1b9b094c785fb1fabf9342f43ee73</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>AbstractPhronesisClient</name>
    <filename>classAbstractPhronesisClient.html</filename>
    <base>Observer</base>
    <member kind="function">
      <type></type>
      <name>AbstractPhronesisClient</name>
      <anchorfile>classAbstractPhronesisClient.html</anchorfile>
      <anchor>acaa0288fc4b867cf5db93f854afbdc31</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>AbstractPhronesisClient</name>
      <anchorfile>classAbstractPhronesisClient.html</anchorfile>
      <anchor>a2723276a809f376fb2a6814f5d933bad</anchor>
      <arglist>(std::string serverAddr, std::string port)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isConnected</name>
      <anchorfile>classAbstractPhronesisClient.html</anchorfile>
      <anchor>a5afaf9822e124a67317f6d7d11ec2e04</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>update</name>
      <anchorfile>classAbstractPhronesisClient.html</anchorfile>
      <anchor>ace497c519ed4d6f4d97799b5624177f2</anchor>
      <arglist>(api_message msg)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>answerQuestion</name>
      <anchorfile>classAbstractPhronesisClient.html</anchorfile>
      <anchor>a1488a5d408e9b3c17cd1133c74be8194</anchor>
      <arglist>(int id, std::string answer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>ackMessage</name>
      <anchorfile>classAbstractPhronesisClient.html</anchorfile>
      <anchor>a09242df0fd4ba434c0306472f10dfd14</anchor>
      <arglist>(int id)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>giveProblemList</name>
      <anchorfile>classAbstractPhronesisClient.html</anchorfile>
      <anchor>a6733053e75f871b5fd99a9fe348e7a9f</anchor>
      <arglist>(std::string pb)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setVeto</name>
      <anchorfile>classAbstractPhronesisClient.html</anchorfile>
      <anchor>a32e2ff09a4029b96e2946137c5c922f9</anchor>
      <arglist>(std::string vetoName, bool vetoValue)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>loop</name>
      <anchorfile>classAbstractPhronesisClient.html</anchorfile>
      <anchor>a953d63133a7b32eeed7b0e790900181f</anchor>
      <arglist>(api_client &amp;c)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>quit</name>
      <anchorfile>classAbstractPhronesisClient.html</anchorfile>
      <anchor>ab28b7c4fef172ec50853a36e7456a25e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>defaultRun</name>
      <anchorfile>classAbstractPhronesisClient.html</anchorfile>
      <anchor>a3061783599c147d985f1d9a4175d2dd9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>run</name>
      <anchorfile>classAbstractPhronesisClient.html</anchorfile>
      <anchor>ac0067eec6d2adb6d92fc566f16e11d7e</anchor>
      <arglist>(std::string serverAddr, std::string port)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>sendOneMessage</name>
      <anchorfile>classAbstractPhronesisClient.html</anchorfile>
      <anchor>a66d9621b5533968510c796d2868683f9</anchor>
      <arglist>(api_message msg)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>api_client *</type>
      <name>m_c</name>
      <anchorfile>classAbstractPhronesisClient.html</anchorfile>
      <anchor>a72d7e070b65f8f096c2c18dd552ad867</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>m_quit</name>
      <anchorfile>classAbstractPhronesisClient.html</anchorfile>
      <anchor>a04b8ce1a79d272b2f1ab0fb0b40bbfbe</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::string</type>
      <name>m_serverAddr</name>
      <anchorfile>classAbstractPhronesisClient.html</anchorfile>
      <anchor>a8b9abfceaccae795f8f141fd573985d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::string</type>
      <name>m_port</name>
      <anchorfile>classAbstractPhronesisClient.html</anchorfile>
      <anchor>a504d9d94fb863bf8ecfa8b12c6b8f040</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>answerQuestion</name>
    <filename>classanswerQuestion.html</filename>
    <base>AbstractPhronesisClient</base>
    <member kind="function">
      <type></type>
      <name>answerQuestion</name>
      <anchorfile>classanswerQuestion.html</anchorfile>
      <anchor>aac8efcac2cde38104ef1ad86004e3156</anchor>
      <arglist>(int id, std::string text)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>loop</name>
      <anchorfile>classanswerQuestion.html</anchorfile>
      <anchor>aaa1d717c9a705cc00d49a5cb3f2d4ca1</anchor>
      <arglist>(api_client &amp;c)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update</name>
      <anchorfile>classanswerQuestion.html</anchorfile>
      <anchor>ad9629e7a2b44cbbe47c780388add7fc1</anchor>
      <arglist>(api_message msg)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>int</type>
      <name>m_id</name>
      <anchorfile>classanswerQuestion.html</anchorfile>
      <anchor>a3565306f9fa2700ff314f7a997d75584</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::string</type>
      <name>m_text</name>
      <anchorfile>classanswerQuestion.html</anchorfile>
      <anchor>a9825e63aea58022f1bb4c25d49c7c3df</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>api_client</name>
    <filename>classapi__client.html</filename>
    <base>Observable</base>
    <member kind="function">
      <type></type>
      <name>api_client</name>
      <anchorfile>classapi__client.html</anchorfile>
      <anchor>a0799833ed0260c95a338b500d6ac0fc7</anchor>
      <arglist>(boost::asio::io_service &amp;io_service, tcp::endpoint &amp;endpoint)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write</name>
      <anchorfile>classapi__client.html</anchorfile>
      <anchor>a0c26d59b6efaac9b25b0a52c471f1210</anchor>
      <arglist>(api_message &amp;msg)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>close</name>
      <anchorfile>classapi__client.html</anchorfile>
      <anchor>aec7aa69810be774d40c3c0b7d123b9ac</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isConnected</name>
      <anchorfile>classapi__client.html</anchorfile>
      <anchor>a6d714d0c68dad27099ca162689da337f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>handle_connect</name>
      <anchorfile>classapi__client.html</anchorfile>
      <anchor>a4db37254458997f23bcecd8bed9b61a2</anchor>
      <arglist>(const boost::system::error_code &amp;error)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>handle_read</name>
      <anchorfile>classapi__client.html</anchorfile>
      <anchor>a069fb2fd095a52c10628f9b503fb47f9</anchor>
      <arglist>(const boost::system::error_code &amp;error)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>handle_write</name>
      <anchorfile>classapi__client.html</anchorfile>
      <anchor>aa40772357a8efbb5bdd94c313a59f728</anchor>
      <arglist>(const boost::system::error_code &amp;error)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>do_close</name>
      <anchorfile>classapi__client.html</anchorfile>
      <anchor>a60645d8ea945c67200d7e60b5cb444c8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>boost::asio::io_service &amp;</type>
      <name>m_io_service</name>
      <anchorfile>classapi__client.html</anchorfile>
      <anchor>ae925de31bff7bf763b1e473ce5298543</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>connection_ptr</type>
      <name>m_tcp_connection</name>
      <anchorfile>classapi__client.html</anchorfile>
      <anchor>a0340ce904a3798f80c510f7d1e817c94</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>tcp::endpoint &amp;</type>
      <name>m_endpoint</name>
      <anchorfile>classapi__client.html</anchorfile>
      <anchor>a831a33eb8d219bbeb23dfc2178d0050f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>api_message</type>
      <name>m_message_read</name>
      <anchorfile>classapi__client.html</anchorfile>
      <anchor>a4be004ff32f72b28a763663066dc9966</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>m_isConnected</name>
      <anchorfile>classapi__client.html</anchorfile>
      <anchor>ab0aec8bba6978b6840206c95f38ebf63</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>m_quit</name>
      <anchorfile>classapi__client.html</anchorfile>
      <anchor>af2190a3297251a694be3c292cce4bdfa</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>coutConsole</name>
    <filename>classcoutConsole.html</filename>
    <base>AbstractPhronesisClient</base>
    <member kind="function">
      <type>void</type>
      <name>update</name>
      <anchorfile>classcoutConsole.html</anchorfile>
      <anchor>a327cd66852daf7be323fcd4fdf19292a</anchor>
      <arglist>(api_message e)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Observable</name>
    <filename>classObservable.html</filename>
    <member kind="function">
      <type>void</type>
      <name>notify</name>
      <anchorfile>classObservable.html</anchorfile>
      <anchor>adf9d5e4f0775bdf44ac48fb382294152</anchor>
      <arglist>(api_message data)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_observer</name>
      <anchorfile>classObservable.html</anchorfile>
      <anchor>a03cb6685753bc604ca3fd958c73fc305</anchor>
      <arglist>(Observer *observer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>del_observer</name>
      <anchorfile>classObservable.html</anchorfile>
      <anchor>ace544a1de9dde09f79c831fd7f1cb925</anchor>
      <arglist>(Observer *observer)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::list&lt; Observer * &gt;</type>
      <name>list_observers</name>
      <anchorfile>classObservable.html</anchorfile>
      <anchor>a1947d597a065dcb134a3627e3c52c5b9</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Observer</name>
    <filename>classObserver.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~Observer</name>
      <anchorfile>classObserver.html</anchorfile>
      <anchor>afcc6b67be6c386f2f3d2c363aa59cb47</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>update</name>
      <anchorfile>classObserver.html</anchorfile>
      <anchor>a81bcc1d4eb4114406e36355a6cdf8be0</anchor>
      <arglist>(api_message data)=0</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ObserverWrap</name>
    <filename>classObserverWrap.html</filename>
    <base>Observer</base>
    <member kind="function">
      <type>void</type>
      <name>update</name>
      <anchorfile>classObserverWrap.html</anchorfile>
      <anchor>a430e9f166faff176bbe674108d627fa7</anchor>
      <arglist>(api_message data)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>phrUtilsInterface</name>
    <filename>classphrUtilsInterface.html</filename>
    <base>AbstractPhronesisClient</base>
    <member kind="function">
      <type></type>
      <name>phrUtilsInterface</name>
      <anchorfile>classphrUtilsInterface.html</anchorfile>
      <anchor>ac17bf933d2d2d08a00a22637166a785a</anchor>
      <arglist>(std::string serverAddr, std::string port, int mode=OBSERVE)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update</name>
      <anchorfile>classphrUtilsInterface.html</anchorfile>
      <anchor>afea0592ea3cd01ea2d059de2c76b5bd8</anchor>
      <arglist>(api_message e)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>m_mode</name>
      <anchorfile>classphrUtilsInterface.html</anchorfile>
      <anchor>ad3a20c3c2cda3c744cded49ad8805c28</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>pyAbstractPhronesisClient</name>
    <filename>classpyAbstractPhronesisClient.html</filename>
    <base>AbstractPhronesisClient</base>
    <member kind="function">
      <type></type>
      <name>pyAbstractPhronesisClient</name>
      <anchorfile>classpyAbstractPhronesisClient.html</anchorfile>
      <anchor>ae456fc94681463736f02f724a94a426c</anchor>
      <arglist>(PyObject *self)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>pyAbstractPhronesisClient</name>
      <anchorfile>classpyAbstractPhronesisClient.html</anchorfile>
      <anchor>adca4c063c4d354e6a9eb74f982e327dd</anchor>
      <arglist>(PyObject *self, const AbstractPhronesisClient &amp;api)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update</name>
      <anchorfile>classpyAbstractPhronesisClient.html</anchorfile>
      <anchor>a9764ac7a152f3457aceda7be2a26ec17</anchor>
      <arglist>(api_message e)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>PyObject *const</type>
      <name>m_self</name>
      <anchorfile>classpyAbstractPhronesisClient.html</anchorfile>
      <anchor>ab6f8b239f337807d09fbfc2aa8ebbf81</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>SetVetoInterface</name>
    <filename>classSetVetoInterface.html</filename>
    <base>AbstractPhronesisClient</base>
    <member kind="function">
      <type></type>
      <name>SetVetoInterface</name>
      <anchorfile>classSetVetoInterface.html</anchorfile>
      <anchor>ac5d48efa4a4946fffaa5e6b2f8e60eeb</anchor>
      <arglist>(std::string name, bool value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>loop</name>
      <anchorfile>classSetVetoInterface.html</anchorfile>
      <anchor>af8e26c8920fb4884063cd7615722f6dc</anchor>
      <arglist>(api_client &amp;c)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>update</name>
      <anchorfile>classSetVetoInterface.html</anchorfile>
      <anchor>a6a46be83dca5af29357342c0cb745a99</anchor>
      <arglist>(api_message e)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>m_value</name>
      <anchorfile>classSetVetoInterface.html</anchorfile>
      <anchor>a4361f295234a2af2a306d2a0f7f2b990</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::string</type>
      <name>m_name</name>
      <anchorfile>classSetVetoInterface.html</anchorfile>
      <anchor>aa81709aada7e7cee8308ed72b4f52707</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>setup</name>
    <filename>namespacesetup.html</filename>
    <member kind="variable">
      <type>list</type>
      <name>ext_modules</name>
      <anchorfile>namespacesetup.html</anchorfile>
      <anchor>a75e1b9b094c785fb1fabf9342f43ee73</anchor>
      <arglist></arglist>
    </member>
  </compound>
</tagfile>
