<?xml version='1.0' encoding='ISO-8859-1' standalone='yes' ?>
<tagfile>
  <compound kind="file">
    <name>PhronesisAgent.hpp</name>
    <path>/home/chaen/phronesis/src/analyzer/analyzerAgent/</path>
    <filename>PhronesisAgent_8hpp</filename>
    <includes id="Treatment_8hpp" name="Treatment.hpp" local="yes" imported="no">Treatment.hpp</includes>
    <class kind="class">PhronesisAgent</class>
  </compound>
  <compound kind="file">
    <name>Treatment.hpp</name>
    <path>/home/chaen/phronesis/src/analyzer/analyzerAgent/</path>
    <filename>Treatment_8hpp</filename>
    <class kind="class">Treatment</class>
    <member kind="typedef">
      <type>boost::shared_ptr&lt; Treatment &gt;</type>
      <name>Treatment_ptr</name>
      <anchorfile>Treatment_8hpp.html</anchorfile>
      <anchor>ae1590bb18f2b2a395b59bff17d314e48</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char</type>
      <name>LIMIT_NAMES</name>
      <anchorfile>Treatment_8hpp.html</anchorfile>
      <anchor>af25aae79c3eb5d470377e300a3b3f1b3</anchor>
      <arglist>[16][22]</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PhronesisAgent</name>
    <filename>classPhronesisAgent.html</filename>
    <member kind="function">
      <type></type>
      <name>PhronesisAgent</name>
      <anchorfile>classPhronesisAgent.html</anchorfile>
      <anchor>aa006e61a2738a1f5193b0ec9b791cd3c</anchor>
      <arglist>(boost::asio::io_service &amp;io_service, const tcp::endpoint &amp;endpoint)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>wait_for_connection</name>
      <anchorfile>classPhronesisAgent.html</anchorfile>
      <anchor>a8989ac273b09d24ec82ea8a7a976682b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>handle_accept</name>
      <anchorfile>classPhronesisAgent.html</anchorfile>
      <anchor>a1213728cad3a65af5ede04c92aecc63a</anchor>
      <arglist>(const boost::system::error_code &amp;error, connection_ptr new_connection)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>boost::asio::io_service &amp;</type>
      <name>m_io_service</name>
      <anchorfile>classPhronesisAgent.html</anchorfile>
      <anchor>aa5b386a16ffe5dd2c8c95b8e49546c07</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>tcp::acceptor</type>
      <name>m_acceptor</name>
      <anchorfile>classPhronesisAgent.html</anchorfile>
      <anchor>ad93744de074cac66e5cd5a51476881c4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Treatment</name>
    <filename>classTreatment.html</filename>
    <member kind="function">
      <type></type>
      <name>~Treatment</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>aba105ad62394193b2d0d332bacae903e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Treatment_ptr</type>
      <name>create</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>aff8a596b37d97eef9f6c29ca989b3741</anchor>
      <arglist>(connection_ptr tcp_connection)</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static int</type>
      <name>command_timeout</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a3f8b057fdfdf6d0481f564c75002e100</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" protection="private">
      <type></type>
      <name>Treatment</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a37788cd3a2a807563fc5bd1ffbc00bea</anchor>
      <arglist>(connection_ptr tcp_connection)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>wait_for_data</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>af2c13cbdd87b375da8772fbb8c05d720</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>handle_write</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>abae2ec97f53d64bf985eac7a558feea0</anchor>
      <arglist>(const boost::system::error_code &amp;error)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>handle_read</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>ac4e5a5998679bab64fe97e47c77337f8</anchor>
      <arglist>(const boost::system::error_code &amp;error)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>CommandThread</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a5b23f86b5b434bab31dd86f805127e5a</anchor>
      <arglist>(std::string &amp;ret, std::string exec)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>CommandThreadChild</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a2c28bbf4ba40ef86d65d9ede3f888f0a</anchor>
      <arglist>(std::string &amp;ret, std::string exec)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>std::string</type>
      <name>executeCommand</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>ae669641339ce79374a1a1a8c654a4be6</anchor>
      <arglist>(std::string command)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>call</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a9779d09db66f165cff77e86b476a05ff</anchor>
      <arglist>(RET(Treatment::*f)(ARG1 &amp;, ARG2 &amp;), ARG1 &amp;arg1, ARG2 &amp;arg2, std::string errorKey, RET &amp;ret, std::map&lt; std::string, std::string &gt; &amp;errorMap)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>call</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>aa4b834501930e6f523c4c067f86a46d5</anchor>
      <arglist>(RET(Treatment::*f)(ARG &amp;), ARG &amp;arg, std::string errorKey, RET &amp;ret, std::map&lt; std::string, std::string &gt; &amp;errorMap)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>void</type>
      <name>call</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a90acf00c6a2a9a05af585c995b6f282c</anchor>
      <arglist>(RET(Treatment::*f)(), std::string errorKey, RET &amp;ret, std::map&lt; std::string, std::string &gt; &amp;errorMap)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>std::string</type>
      <name>remove_excessive_spaces</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a404b8ebda30bca13a2ad753fc7e76ea6</anchor>
      <arglist>(const std::string &amp;s)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>float</type>
      <name>getNumberOfProcessor</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>aa823bfcfaeb9e21cc6c295192f0e656a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>float</type>
      <name>env_getLoad</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>af0b481dfd1df617693ac5d4577c7d97e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>float</type>
      <name>env_getUsedMemory</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a383db3fd7fc64809686260fd778e5590</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>float</type>
      <name>env_getUsedSwap</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>af4fbb557b9620a04f847b222ae3b080a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>std::map&lt; std::string, float &gt;</type>
      <name>env_getUsedDiskSpace</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a4287dba03f7b9f0e0e4099a2bbe38757</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>std::map&lt; std::string, float &gt;</type>
      <name>env_getUsedInodes</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a14c1101e5d0ed3ff5f01d02ddf69d6ad</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>std::vector&lt; triplet&lt; std::string, float, std::string &gt; &gt;</type>
      <name>env_getTopCpuConsumers</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a9a35b63d96f1abd738e3e67e6fcb50f7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>std::vector&lt; triplet&lt; std::string, float, std::string &gt; &gt;</type>
      <name>env_getTopMemoryConsumers</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>ae6b9e8ffe2fbac48761875fc7342c52c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="private">
      <type>MountpointInfo</type>
      <name>mount_CheckMountPointForFile</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a96fdcba8c52311ac7a24b89841f6222a</anchor>
      <arglist>(std::string &amp;path)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>bool</type>
      <name>mount_isMountpointMounted</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a80a1a47f803167e9cab3afd3f8630541</anchor>
      <arglist>(MountpointInfo &amp;mountpoint)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>bool</type>
      <name>mount_isMountPointReadWriteForUser</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a39fc0e3e9aade3290fcb8ac4b8132730</anchor>
      <arglist>(MountpointInfo &amp;mountpoint, std::string &amp;uid)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>std::map&lt; int, std::vector&lt; int &gt; &gt;</type>
      <name>proc_getPidTree</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a4b5f696eb54f1c93bd0df6e6c8b6b4a2</anchor>
      <arglist>(std::string &amp;procName)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>std::vector&lt; std::string &gt;</type>
      <name>proc_getPsLine</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>ad03dab1124be41489af5881f8a6b7858</anchor>
      <arglist>(std::string &amp;pid)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>std::string</type>
      <name>proc_getFullCommandLine</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a593f66f4a8a43bed247cffbf76a67f9c</anchor>
      <arglist>(std::string &amp;pid)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>std::string</type>
      <name>proc_getProcessUser</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a7c2e2b7c78e4d4b39c3b44ac88ab1560</anchor>
      <arglist>(std::string &amp;pid)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>float</type>
      <name>proc_getProcessCpuUsage</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>aa0e79fd1bc23aa7b53609acef9ccf68a</anchor>
      <arglist>(std::string &amp;pid)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>float</type>
      <name>proc_getProcessMemoryUsage</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a39bf289ef9e59c3195d6d5ad86793ba2</anchor>
      <arglist>(std::string &amp;pid)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>std::map&lt; std::string, std::pair&lt; unsigned long int, unsigned long int &gt; &gt;</type>
      <name>proc_getProcessLimits</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>af4cb29e108ee07aeb995e55c1b42c229</anchor>
      <arglist>(std::string &amp;pid)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>std::map&lt; std::string, unsigned long int &gt;</type>
      <name>proc_getProcessLimitStatus</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a05dd459171e613e55c0fef44257e1dd4</anchor>
      <arglist>(std::string &amp;pid)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>bool</type>
      <name>file_fileExists</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a4bd69fedec12d9d6ade02870ebcdf74d</anchor>
      <arglist>(std::string &amp;fileName)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>std::string</type>
      <name>file_getFileOwner</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a15d4239c2f1b221604d608e20aa2706e</anchor>
      <arglist>(std::string &amp;fileName)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>std::string</type>
      <name>file_getFileGroup</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a0027860c7b931398969a9b8844ed8501</anchor>
      <arglist>(std::string &amp;fileName)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>std::string</type>
      <name>file_getFilePermissions</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>af22094d876998c713f82c20fab91efc0</anchor>
      <arglist>(std::string &amp;fileName)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>std::string</type>
      <name>file_getFileAttributes</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>abd48e785f4eb24f3e343737bb1a362bf</anchor>
      <arglist>(std::string &amp;fileName)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>std::string</type>
      <name>file_getFileMd5</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a79e4864f466c67b879fd88a6acdcae92</anchor>
      <arglist>(std::string &amp;fileName)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>std::map&lt; std::string, bool &gt;</type>
      <name>file_getFileRegex</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>afbcfa350bf105fe753e44063d0c69811</anchor>
      <arglist>(std::string &amp;fileName, std::vector&lt; std::string &gt; &amp;regex)</arglist>
    </member>
    <member kind="function" protection="private">
      <type>std::vector&lt; std::string &gt;</type>
      <name>folder_getFolderContent</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a27e1ef357f98d6c6fc494ddc4b9126c2</anchor>
      <arglist>(std::string &amp;folderName, std::map&lt; std::string, std::string &gt; &amp;extraArgs)</arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::string</type>
      <name>m_cpuinfo</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a338b712203878779eaffac878c2206fb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::string</type>
      <name>m_loadavg</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>adb0a850fcf74d8edc56e59c7415a53fb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>std::string</type>
      <name>m_meminfo</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a9e24db0d8a75b290bd318f6774531488</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>connection_ptr</type>
      <name>m_tcp_connection</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a96ec10541bfd37891618e7803fc1593a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>Request</type>
      <name>m_request</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a5e64678e817bf627df1566ab79206166</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>bool</type>
      <name>is_leaving</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a614ade752acc932ec9b134fd49d0d62a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>boost::thread::id</type>
      <name>m_currentCommandThreadId</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a217b1e86efc3a48065eee7b530bb0a06</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="private">
      <type>boost::mutex</type>
      <name>m_threadIdMutex</name>
      <anchorfile>classTreatment.html</anchorfile>
      <anchor>a86d4ad2b5c02435ce6bfd03b86327e10</anchor>
      <arglist></arglist>
    </member>
  </compound>
</tagfile>
