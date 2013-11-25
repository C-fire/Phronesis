<?xml version='1.0' encoding='ISO-8859-1' standalone='yes' ?>
<tagfile>
  <compound kind="file">
    <name>AnalyzerDBAPI.hpp</name>
    <path>/home/chaen/phronesis/src/analyzer/analyzerDBAPI/</path>
    <filename>AnalyzerDBAPI_8hpp</filename>
    <class kind="class">AnalyzerDBAPI</class>
    <member kind="define">
      <type>#define</type>
      <name>USE_MYSQL</name>
      <anchorfile>AnalyzerDBAPI_8hpp.html</anchorfile>
      <anchor>ac1be9e2f742b371c01266bfc82d198c4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>FOR_EACH_LINE</name>
      <anchorfile>AnalyzerDBAPI_8hpp.html</anchorfile>
      <anchor>ad5809977066f9c0845359e562f7b76e7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>FOR_EACH_COL</name>
      <anchorfile>AnalyzerDBAPI_8hpp.html</anchorfile>
      <anchor>a4762522c824527831cfdcf4706f990e2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>FOR_EACH</name>
      <anchorfile>AnalyzerDBAPI_8hpp.html</anchorfile>
      <anchor>a24b5e5cbd3714bb277129f9ceaa2fec0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DONE_COL</name>
      <anchorfile>AnalyzerDBAPI_8hpp.html</anchorfile>
      <anchor>a62c9a6e6ceb80f871b3962e4bb0d86fc</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DONE_LINE</name>
      <anchorfile>AnalyzerDBAPI_8hpp.html</anchorfile>
      <anchor>ad3930d4d81f3faf014d7ee2d8f122667</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DONE</name>
      <anchorfile>AnalyzerDBAPI_8hpp.html</anchorfile>
      <anchor>abe6b865c045f3e7c6892ef4f15ff5779</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>AnalyzerDBAPI</name>
    <filename>classAnalyzerDBAPI.html</filename>
    <member kind="function">
      <type></type>
      <name>AnalyzerDBAPI</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a7cbc8c88e71297bd8cfe24da530390c5</anchor>
      <arglist>(string type, string server, string user, string passwd, int port, string name)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~AnalyzerDBAPI</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a8a8915e65ca46dfe554a2263f61df999</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>retrieveAllAgents</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a5d43458cbb9764c5981630aac67d1699</anchor>
      <arglist>(vector&lt; Agent * &gt; &amp;inV, map&lt; string, Server * &gt; servers)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>retrieveAllCoordinators</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>ae1b9b04ac2f05094d93eb7aa7e9b1eb2</anchor>
      <arglist>(vector&lt; Coordinator * &gt; &amp;inV)</arglist>
    </member>
    <member kind="function">
      <type>vector&lt; vector&lt; string &gt; &gt;</type>
      <name>getTree</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>aeac9745c1ff60a08b58b8487c5e75d27</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>vector&lt; vector&lt; string &gt; &gt;</type>
      <name>getServerAttachedCoordinators</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a1163add743a44a42ab5d55553fa5c0c6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getOccurrence</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a5665a084dbe8bb5c146558e6fd3a081c</anchor>
      <arglist>(int id_parent, int id_daugther)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getTotal</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a5cf333d8b1766658ef0d48eb151a61af</anchor>
      <arglist>(int id_parent)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addUnsolved</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a3eac71479217e30272cb0d10b2436da6</anchor>
      <arglist>(int id_agent)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>incrementOccurrence</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>ad4fa7acb6d8bdc9cff9f0d4275e49001</anchor>
      <arglist>(int pId, int dId)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>incrementTotal</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>af2673a58dbff6972ba5a0fe623364069</anchor>
      <arglist>(int pId)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>incrementLocalTotal</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a69d88578e515e49194c661918b10dc10</anchor>
      <arglist>(int pId)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>incrementDependencyOccurence</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a95c4b5cdc31a380ff2b1b3131b19368b</anchor>
      <arglist>(int id_prior, int id_after)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getDependencyOccurence</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>ab57d5dd810876454b7721d5cfe05d4dd</anchor>
      <arglist>(int id_prior, int id_after)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>validateDependency</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a9044f741ed313fde1f7c9ac2e6c2903d</anchor>
      <arglist>(int id_prior, int id_after)</arglist>
    </member>
    <member kind="function">
      <type>vector&lt; string &gt;</type>
      <name>getAfters</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a4a98570353987bc697745bdad7ea8373</anchor>
      <arglist>(int id_prior)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::string &gt;</type>
      <name>getBefores</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a9a84cd4ca1161a5921d47ead830ae9d4</anchor>
      <arglist>(int id_after)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>retrieveAllServers</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>aef86e91cff06537bc4d4a61715d57f21</anchor>
      <arglist>(map&lt; string, Server * &gt; &amp;servers)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>incrementServerFailure</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a125a82b30db727d88a921adb0c58d596</anchor>
      <arglist>(int sId)</arglist>
    </member>
    <member kind="function">
      <type>map&lt; pair&lt; string, string &gt;, int &gt;</type>
      <name>getRecoveryRulesMap</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a1e3846850d4214477a03dd3c54dfa144</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>vector&lt; pair&lt; int, int &gt; &gt;</type>
      <name>getRecoveryRulesTree</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>aea7ca7e3ef26e712ce81ce5bb77821da</anchor>
      <arglist>(map&lt; pair&lt; string, string &gt;, int &gt; rulesMap, string direction=&quot;&quot;)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; RecoveryHistory &gt;</type>
      <name>getRecoveryHistory</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a392eb27fa467ed8a6457e8a02eafb27a</anchor>
      <arglist>(std::string responsibleName)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addRecoveryHistoryEvent</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a53125c35794ec961e2190594f7c6a68b</anchor>
      <arglist>(RecoveryHistory event)</arglist>
    </member>
    <member kind="enumeration">
      <name>DbType</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a966570058d39c4db231f9500724dc958</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>connect</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>ad29e17a67d7b7b90a7cf59b8a9ff7634</anchor>
      <arglist>(string server, string user, string passwd, int port, string name)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>int</type>
      <name>executeQuery</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a4380fcb24e511f99c3fb262259b7f778</anchor>
      <arglist>(string Query, bool printReq=false, bool retry=false)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>insertQuery</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>aef559300bbbfad3accb070e2d567ce9d</anchor>
      <arglist>(string query, bool retry=false)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>string</type>
      <name>escapeString</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a9c4f693e03464376c299b4b9366e2dfd</anchor>
      <arglist>(string pStr)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>std::string</type>
      <name>getMaxId</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a150f1d06c8b6d0d532b883768c99dcd7</anchor>
      <arglist>(std::string table)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>DbType</type>
      <name>dbType</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a50f2b20403cf5c332290290fbf16500a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>vector&lt; vector&lt; string &gt; &gt;</type>
      <name>m_queryResult</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a7ad74b44115c8b3bffa68d3a33cb2d3b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>MYSQL</type>
      <name>m_connection</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>a19af1ee76e1601682d2c6727a3428f5f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>MYSQL_RES *</type>
      <name>m_result</name>
      <anchorfile>classAnalyzerDBAPI.html</anchorfile>
      <anchor>af48a3d646806121d22a049db39a3bbab</anchor>
      <arglist></arglist>
    </member>
  </compound>
</tagfile>
