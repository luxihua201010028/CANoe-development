<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="14008000">
	<Property Name="varPersistentID:{1837E0A1-E0C3-4390-B0E9-0212ABF02565}" Type="Ref">/我的电脑/VariablesLibrary.lvlib/CANoe_LabVIEW</Property>
	<Property Name="varPersistentID:{3673B5BC-4B94-401F-9EAD-AFBF5DE742FE}" Type="Ref">/我的电脑/VariablesLibrary.lvlib/LabVIEW_CANoe</Property>
	<Item Name="我的电脑" Type="My Computer">
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">我的电脑/VI服务器</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">我的电脑/VI服务器</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="DemoVI.vi" Type="VI" URL="../DemoVI.vi"/>
		<Item Name="VariablesLibrary.lvlib" Type="Library" URL="../VariablesLibrary.lvlib"/>
		<Item Name="依赖关系" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="Error Cluster From Error Code.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Cluster From Error Code.vi"/>
				<Item Name="ni_tagger_lv_FlushAllConnections.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/ni_tagger_lv_FlushAllConnections.vi"/>
				<Item Name="NI_VariableUtilities.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/Variable/NI_VariableUtilities.lvlib"/>
				<Item Name="subTimeDelay.vi" Type="VI" URL="/&lt;vilib&gt;/express/express execution control/TimeDelayBlock.llb/subTimeDelay.vi"/>
			</Item>
			<Item Name="nitaglv.dll" Type="Document" URL="nitaglv.dll">
				<Property Name="NI.PreserveRelativePath" Type="Bool">true</Property>
			</Item>
		</Item>
		<Item Name="程序生成规范" Type="Build">
			<Item Name="My Application" Type="EXE">
				<Property Name="App_copyErrors" Type="Bool">true</Property>
				<Property Name="App_INI_aliasGUID" Type="Str">{5DACBB59-DD9E-4930-B72C-579F4929B7E2}</Property>
				<Property Name="App_INI_GUID" Type="Str">{BF83A9C0-CC5A-4569-B3CB-DE4DFECEB2CF}</Property>
				<Property Name="App_serverConfig.httpPort" Type="Int">8002</Property>
				<Property Name="Bld_autoIncrement" Type="Bool">true</Property>
				<Property Name="Bld_buildCacheID" Type="Str">{9AD3AAA5-CD0A-4F2B-B8DE-E82FC8465EE3}</Property>
				<Property Name="Bld_buildSpecName" Type="Str">My Application</Property>
				<Property Name="Bld_defaultLanguage" Type="Str">ChineseS</Property>
				<Property Name="Bld_excludeInlineSubVIs" Type="Bool">true</Property>
				<Property Name="Bld_excludeLibraryItems" Type="Bool">true</Property>
				<Property Name="Bld_excludePolymorphicVIs" Type="Bool">true</Property>
				<Property Name="Bld_localDestDir" Type="Path">../My Application</Property>
				<Property Name="Bld_localDestDirType" Type="Str">relativeToProject</Property>
				<Property Name="Bld_modifyLibraryFile" Type="Bool">true</Property>
				<Property Name="Bld_previewCacheID" Type="Str">{3B25546C-DB4C-4224-B789-A0AA201B3243}</Property>
				<Property Name="Bld_version.build" Type="Int">4</Property>
				<Property Name="Bld_version.major" Type="Int">1</Property>
				<Property Name="Destination[0].destName" Type="Str">DemoVI.exe</Property>
				<Property Name="Destination[0].path" Type="Path">../My Application/DemoVI.exe</Property>
				<Property Name="Destination[0].path.type" Type="Str">relativeToProject</Property>
				<Property Name="Destination[0].preserveHierarchy" Type="Bool">true</Property>
				<Property Name="Destination[0].type" Type="Str">App</Property>
				<Property Name="Destination[1].destName" Type="Str">支持目录</Property>
				<Property Name="Destination[1].path" Type="Path">../My Application/data</Property>
				<Property Name="Destination[1].path.type" Type="Str">relativeToProject</Property>
				<Property Name="DestinationCount" Type="Int">2</Property>
				<Property Name="Exe_actXinfo_enumCLSID[0]" Type="Str">{6C10D8C8-6D4B-4A7D-BB2A-BC71A89C1454}</Property>
				<Property Name="Exe_actXinfo_enumCLSID[1]" Type="Str">{25E8945F-494B-4695-B6A6-AFA045EEFF79}</Property>
				<Property Name="Exe_actXinfo_enumCLSID[10]" Type="Str">{09BD161D-AE12-45A4-95D8-CBD5F1321045}</Property>
				<Property Name="Exe_actXinfo_enumCLSID[11]" Type="Str">{9563A3BC-9128-4C4F-A099-F28FC0F17024}</Property>
				<Property Name="Exe_actXinfo_enumCLSID[12]" Type="Str">{4C4E4989-233A-4100-B743-306D9861FFE9}</Property>
				<Property Name="Exe_actXinfo_enumCLSID[13]" Type="Str">{C57678E7-CFAA-4078-9F68-7C08753ADCC9}</Property>
				<Property Name="Exe_actXinfo_enumCLSID[14]" Type="Str">{C5FFF165-E846-4228-B7CE-0ED735F1DF54}</Property>
				<Property Name="Exe_actXinfo_enumCLSID[15]" Type="Str">{255A57EF-F054-424F-BF99-DF10C1B4296B}</Property>
				<Property Name="Exe_actXinfo_enumCLSID[16]" Type="Str">{0FA2C568-F6AB-414F-B28B-6F240F83F809}</Property>
				<Property Name="Exe_actXinfo_enumCLSID[2]" Type="Str">{00A36234-B34D-4783-A749-949E7BB58743}</Property>
				<Property Name="Exe_actXinfo_enumCLSID[3]" Type="Str">{ED728DBB-59B2-49FC-B4F5-A395BE381130}</Property>
				<Property Name="Exe_actXinfo_enumCLSID[4]" Type="Str">{1D2A2785-B0DD-4C30-8BE5-A764E625579B}</Property>
				<Property Name="Exe_actXinfo_enumCLSID[5]" Type="Str">{1AAA402A-D0D5-478C-9E73-93119E6C00ED}</Property>
				<Property Name="Exe_actXinfo_enumCLSID[6]" Type="Str">{A792C8A0-34CC-435C-ABDC-B8E5A9F271AD}</Property>
				<Property Name="Exe_actXinfo_enumCLSID[7]" Type="Str">{6ED1F883-274B-4FC2-BFBF-C0A655EBB46D}</Property>
				<Property Name="Exe_actXinfo_enumCLSID[8]" Type="Str">{E8000DC3-FF86-40FF-8CD8-C213260B50E5}</Property>
				<Property Name="Exe_actXinfo_enumCLSID[9]" Type="Str">{3799759C-41AD-4991-82C7-D63F749FC5B1}</Property>
				<Property Name="Exe_actXinfo_enumCLSIDsCount" Type="Int">17</Property>
				<Property Name="Exe_actXinfo_majorVersion" Type="Int">5</Property>
				<Property Name="Exe_actXinfo_minorVersion" Type="Int">5</Property>
				<Property Name="Exe_actXinfo_objCLSID[0]" Type="Str">{A44C5212-2C75-45C6-B9D7-29C8FDAA2165}</Property>
				<Property Name="Exe_actXinfo_objCLSID[1]" Type="Str">{9435B01A-2E98-42EC-A8EB-E51B6F00C1B4}</Property>
				<Property Name="Exe_actXinfo_objCLSID[10]" Type="Str">{F8F1931E-AF46-4B71-AC88-FFC42356D600}</Property>
				<Property Name="Exe_actXinfo_objCLSID[11]" Type="Str">{0A558864-1584-4186-ADBC-B6D8BF518712}</Property>
				<Property Name="Exe_actXinfo_objCLSID[12]" Type="Str">{BF144919-FCF6-4E14-BC99-9F6D24998732}</Property>
				<Property Name="Exe_actXinfo_objCLSID[13]" Type="Str">{0BD74073-8AF4-409E-88C7-D262835A7FA4}</Property>
				<Property Name="Exe_actXinfo_objCLSID[2]" Type="Str">{9A4751FC-5C85-449F-A4D8-58CB0EA201DA}</Property>
				<Property Name="Exe_actXinfo_objCLSID[3]" Type="Str">{3B772C04-3B0A-4411-96A6-549D981C48DA}</Property>
				<Property Name="Exe_actXinfo_objCLSID[4]" Type="Str">{8FEE015A-7455-4E27-A7B3-CBC61A3AB84F}</Property>
				<Property Name="Exe_actXinfo_objCLSID[5]" Type="Str">{A6DA5602-CAF9-4295-9A4E-D4CE1FA633A6}</Property>
				<Property Name="Exe_actXinfo_objCLSID[6]" Type="Str">{164EF522-E5C0-4F27-B488-463CAE18DEE7}</Property>
				<Property Name="Exe_actXinfo_objCLSID[7]" Type="Str">{918EA485-306B-4338-A874-F21DF61D4F21}</Property>
				<Property Name="Exe_actXinfo_objCLSID[8]" Type="Str">{8876BB22-8671-4D08-A194-12880DC3CEC3}</Property>
				<Property Name="Exe_actXinfo_objCLSID[9]" Type="Str">{F35F59FF-B2E7-4139-8132-03AD1ED1084F}</Property>
				<Property Name="Exe_actXinfo_objCLSIDsCount" Type="Int">14</Property>
				<Property Name="Exe_actXinfo_progIDPrefix" Type="Str">DemoVI</Property>
				<Property Name="Exe_actXServerName" Type="Str">DemoVI</Property>
				<Property Name="Exe_actXServerNameGUID" Type="Str">{22F32934-47D2-42C1-A412-D95AA18D4B9E}</Property>
				<Property Name="Source[0].itemID" Type="Str">{AD125A8C-AF82-4C1C-B8F3-E16EE24DF21F}</Property>
				<Property Name="Source[0].type" Type="Str">Container</Property>
				<Property Name="Source[1].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[1].itemID" Type="Ref">/我的电脑/DemoVI.vi</Property>
				<Property Name="Source[1].sourceInclusion" Type="Str">TopLevel</Property>
				<Property Name="Source[1].type" Type="Str">VI</Property>
				<Property Name="SourceCount" Type="Int">2</Property>
				<Property Name="TgtF_companyName" Type="Str">china</Property>
				<Property Name="TgtF_fileDescription" Type="Str">My Application</Property>
				<Property Name="TgtF_internalName" Type="Str">My Application</Property>
				<Property Name="TgtF_legalCopyright" Type="Str">版权 2018 china</Property>
				<Property Name="TgtF_productName" Type="Str">My Application</Property>
				<Property Name="TgtF_targetfileGUID" Type="Str">{E16891B7-62B5-41F3-AFE9-7AD68EFB8024}</Property>
				<Property Name="TgtF_targetfileName" Type="Str">DemoVI.exe</Property>
			</Item>
		</Item>
	</Item>
</Project>
