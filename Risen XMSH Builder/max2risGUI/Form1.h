#pragma once

#include "about.h"
#include "update_bin.h"
#include "output.h"
#include <cstring>
#include <stdlib.h>
#include "file.h"

namespace max2risGUI {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Zusammenfassung für Form1
	///
	/// Warnung: Wenn Sie den Namen dieser Klasse ändern, müssen Sie auch
	///          die Ressourcendateiname-Eigenschaft für das Tool zur Kompilierung verwalteter Ressourcen ändern,
	///          das allen RESX-Dateien zugewiesen ist, von denen diese Klasse abhängt.
	///          Anderenfalls können die Designer nicht korrekt mit den lokalisierten Ressourcen
	///          arbeiten, die diesem Formular zugewiesen sind.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Konstruktorcode hier hinzufügen.
			//
		}

	protected:
		/// <summary>
		/// Verwendete Ressourcen bereinigen.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
	protected: 

	private: System::Windows::Forms::ToolStripMenuItem^  helpToolStripMenuItem;



	private: System::Windows::Forms::ToolStripMenuItem^  languageToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  openToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;

	private: System::Windows::Forms::ToolStripMenuItem^  englishToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  germanToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  aboutToolStripMenuItem;
	
	private: about ^aw;
	private: update_bin ^ubw;
	private: System::Diagnostics::Process ^process;
	//private: System::EventHandler ^process_event_handler;


	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  textBox2;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::SaveFileDialog^  saveFileDialog1;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
	private: System::Windows::Forms::ToolStripMenuItem^  saveToolStripMenuItem;

	private: System::Windows::Forms::ToolStripMenuItem^  convertToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  toolsToolStripMenuItem;

	private: System::Windows::Forms::ToolStripMenuItem^  scriptsToolStripMenuItem;
	private: System::Windows::Forms::FolderBrowserDialog^  folderBrowserDialog1;

	private: file *prefsFile;
	private: System::Windows::Forms::ToolStripMenuItem^  binToolStripMenuItem;
	private: System::Windows::Forms::NumericUpDown^  numericUpDown1;
	private: System::Windows::Forms::CheckBox^  checkBox1;



	private:
		/// <summary>
		/// Erforderliche Designervariable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Erforderliche Methode für die Designerunterstützung.
		/// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->openToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->saveToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->convertToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->languageToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->englishToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->germanToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->scriptsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->binToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->helpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->aboutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->saveFileDialog1 = (gcnew System::Windows::Forms::SaveFileDialog());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->folderBrowserDialog1 = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->process = (gcnew System::Diagnostics::Process());
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->numericUpDown1 = (gcnew System::Windows::Forms::NumericUpDown());
			this->menuStrip1->SuspendLayout();
			this->groupBox1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->BeginInit();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->BackColor = System::Drawing::SystemColors::Control;
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->fileToolStripMenuItem, 
				this->languageToolStripMenuItem, this->toolsToolStripMenuItem, this->helpToolStripMenuItem});
			resources->ApplyResources(this->menuStrip1, L"menuStrip1");
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->ItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &Form1::menuStrip1_ItemClicked);
			// 
			// fileToolStripMenuItem
			// 
			this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->openToolStripMenuItem, 
				this->saveToolStripMenuItem, this->toolStripSeparator1, this->convertToolStripMenuItem});
			this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			resources->ApplyResources(this->fileToolStripMenuItem, L"fileToolStripMenuItem");
			// 
			// openToolStripMenuItem
			// 
			resources->ApplyResources(this->openToolStripMenuItem, L"openToolStripMenuItem");
			this->openToolStripMenuItem->Name = L"openToolStripMenuItem";
			this->openToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::openToolStripMenuItem_Click);
			// 
			// saveToolStripMenuItem
			// 
			resources->ApplyResources(this->saveToolStripMenuItem, L"saveToolStripMenuItem");
			this->saveToolStripMenuItem->Name = L"saveToolStripMenuItem";
			this->saveToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::speichernToolStripMenuItem_Click);
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			resources->ApplyResources(this->toolStripSeparator1, L"toolStripSeparator1");
			// 
			// convertToolStripMenuItem
			// 
			resources->ApplyResources(this->convertToolStripMenuItem, L"convertToolStripMenuItem");
			this->convertToolStripMenuItem->Name = L"convertToolStripMenuItem";
			this->convertToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::toolStripMenuItem2_Click);
			// 
			// languageToolStripMenuItem
			// 
			this->languageToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->englishToolStripMenuItem, 
				this->germanToolStripMenuItem});
			this->languageToolStripMenuItem->Name = L"languageToolStripMenuItem";
			resources->ApplyResources(this->languageToolStripMenuItem, L"languageToolStripMenuItem");
			this->languageToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::languageToolStripMenuItem_Click);
			// 
			// englishToolStripMenuItem
			// 
			resources->ApplyResources(this->englishToolStripMenuItem, L"englishToolStripMenuItem");
			this->englishToolStripMenuItem->Name = L"englishToolStripMenuItem";
			this->englishToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::englischToolStripMenuItem_Click);
			// 
			// germanToolStripMenuItem
			// 
			resources->ApplyResources(this->germanToolStripMenuItem, L"germanToolStripMenuItem");
			this->germanToolStripMenuItem->Name = L"germanToolStripMenuItem";
			this->germanToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::deutschToolStripMenuItem_Click);
			// 
			// toolsToolStripMenuItem
			// 
			this->toolsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->scriptsToolStripMenuItem, 
				this->binToolStripMenuItem});
			this->toolsToolStripMenuItem->Name = L"toolsToolStripMenuItem";
			resources->ApplyResources(this->toolsToolStripMenuItem, L"toolsToolStripMenuItem");
			this->toolsToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::extrasToolStripMenuItem_Click);
			// 
			// scriptsToolStripMenuItem
			// 
			resources->ApplyResources(this->scriptsToolStripMenuItem, L"scriptsToolStripMenuItem");
			this->scriptsToolStripMenuItem->Name = L"scriptsToolStripMenuItem";
			this->scriptsToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::scriptsToolStripMenuItem_Click);
			// 
			// binToolStripMenuItem
			// 
			resources->ApplyResources(this->binToolStripMenuItem, L"binToolStripMenuItem");
			this->binToolStripMenuItem->Name = L"binToolStripMenuItem";
			this->binToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::compiledmeshesbinErweiternToolStripMenuItem_Click);
			// 
			// helpToolStripMenuItem
			// 
			this->helpToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->aboutToolStripMenuItem});
			this->helpToolStripMenuItem->Name = L"helpToolStripMenuItem";
			resources->ApplyResources(this->helpToolStripMenuItem, L"helpToolStripMenuItem");
			// 
			// aboutToolStripMenuItem
			// 
			resources->ApplyResources(this->aboutToolStripMenuItem, L"aboutToolStripMenuItem");
			this->aboutToolStripMenuItem->Name = L"aboutToolStripMenuItem";
			this->aboutToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::aboutToolStripMenuItem_Click);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->button2);
			this->groupBox1->Controls->Add(this->button1);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->textBox2);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->textBox1);
			resources->ApplyResources(this->groupBox1, L"groupBox1");
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->TabStop = false;
			this->groupBox1->Enter += gcnew System::EventHandler(this, &Form1::groupBox1_Enter);
			// 
			// button2
			// 
			resources->ApplyResources(this->button2, L"button2");
			this->button2->Name = L"button2";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &Form1::button2_Click);
			// 
			// button1
			// 
			resources->ApplyResources(this->button1, L"button1");
			this->button1->Name = L"button1";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
			// 
			// label2
			// 
			resources->ApplyResources(this->label2, L"label2");
			this->label2->Name = L"label2";
			// 
			// textBox2
			// 
			resources->ApplyResources(this->textBox2, L"textBox2");
			this->textBox2->Name = L"textBox2";
			// 
			// label1
			// 
			resources->ApplyResources(this->label1, L"label1");
			this->label1->Name = L"label1";
			// 
			// textBox1
			// 
			resources->ApplyResources(this->textBox1, L"textBox1");
			this->textBox1->Name = L"textBox1";
			// 
			// button3
			// 
			resources->ApplyResources(this->button3, L"button3");
			this->button3->Name = L"button3";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &Form1::button3_Click);
			// 
			// saveFileDialog1
			// 
			resources->ApplyResources(this->saveFileDialog1, L"saveFileDialog1");
			this->saveFileDialog1->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &Form1::saveFileDialog1_FileOk);
			// 
			// openFileDialog1
			// 
			resources->ApplyResources(this->openFileDialog1, L"openFileDialog1");
			this->openFileDialog1->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &Form1::openFileDialog1_FileOk);
			// 
			// folderBrowserDialog1
			// 
			resources->ApplyResources(this->folderBrowserDialog1, L"folderBrowserDialog1");
			// 
			// process
			// 
			this->process->EnableRaisingEvents = true;
			this->process->StartInfo->Domain = L"";
			this->process->StartInfo->LoadUserProfile = false;
			this->process->StartInfo->Password = nullptr;
			this->process->StartInfo->StandardErrorEncoding = nullptr;
			this->process->StartInfo->StandardOutputEncoding = nullptr;
			this->process->StartInfo->UserName = L"";
			this->process->SynchronizingObject = this;
			this->process->Exited += gcnew System::EventHandler(this, &Form1::process_exit);
			// 
			// checkBox1
			// 
			resources->ApplyResources(this->checkBox1, L"checkBox1");
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->UseVisualStyleBackColor = true;
			this->checkBox1->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// numericUpDown1
			// 
			resources->ApplyResources(this->numericUpDown1, L"numericUpDown1");
			this->numericUpDown1->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {99, 0, 0, 0});
			this->numericUpDown1->Name = L"numericUpDown1";
			this->numericUpDown1->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {45, 0, 0, 0});
			this->numericUpDown1->ValueChanged += gcnew System::EventHandler(this, &Form1::numericUpDown1_ValueChanged);
			// 
			// Form1
			// 
			resources->ApplyResources(this, L"$this");
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->numericUpDown1);
			this->Controls->Add(this->checkBox1);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->menuStrip1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MainMenuStrip = this->menuStrip1;
			this->MaximizeBox = false;
			this->Name = L"Form1";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &Form1::Form1_FormClosing);
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void Form1_FormClosing(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^  e) {
				prefsFile->jump(0);
				prefsFile->writelong(output::getlanguage());
				char *source = (char *)(void *)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(textBox1->Text);
				char *dest = (char *)(void *)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(textBox2->Text);
				char *bin = (char *)(void *)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(ubw->textBox1->Text);
				char *xmsh = (char *)(void *)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(ubw->textBox2->Text);
				prefsFile->writelong(strlen(source));
				prefsFile->writechararray(source, strlen(source));
				prefsFile->writelong(strlen(dest));
				prefsFile->writechararray(dest, strlen(dest));
				prefsFile->writelong(strlen(bin));
				prefsFile->writechararray(bin, strlen(bin));
				prefsFile->writelong(strlen(xmsh));
				prefsFile->writechararray(xmsh, strlen(xmsh));
				prefsFile->writelong((int) ubw->checkBox1->Checked);
				prefsFile->write();
				System::Runtime::InteropServices::Marshal::FreeHGlobal((System::IntPtr) source);
				System::Runtime::InteropServices::Marshal::FreeHGlobal((System::IntPtr) dest);
				System::Runtime::InteropServices::Marshal::FreeHGlobal((System::IntPtr) bin);
				System::Runtime::InteropServices::Marshal::FreeHGlobal((System::IntPtr) xmsh);
			 }
	private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
			ubw = gcnew update_bin();
			ubw->button3->Click += gcnew System::EventHandler(this, &Form1::ubw_button3_Click);
			aw = gcnew about();
			System::String ^execpath = Application::ExecutablePath;
			System::IO::FileInfo ^fileinfo = gcnew System::IO::FileInfo(execpath);
			System::String ^directory = fileinfo->DirectoryName;
			directory += "\\prefs.rxb";
			char *dir = (char *)(void *)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(directory);
			prefsFile = new file(dir, 3);
			output::setlanguage(prefsFile->readlong());
			if (output::getlanguage() == 1)
			{
				germanToolStripMenuItem->PerformClick();
			}
			else
			{
				englishToolStripMenuItem->PerformClick();
			}
			int len = prefsFile->readlong();
			System::String ^str1 = gcnew System::String(prefsFile->readascii(len));
			textBox1->Text = str1;
			openFileDialog1->FileName = str1;
			len = prefsFile->readlong();
			System::String ^str2 = gcnew System::String(prefsFile->readascii(len));
			textBox2->Text = str2;
			saveFileDialog1->FileName = str2;
			len = prefsFile->readlong();
			System::String ^str3 = gcnew System::String(prefsFile->readascii(len));
			ubw->textBox1->Text = str3;
			ubw->openFileDialog1->FileName = str3;
			len = prefsFile->readlong();
			System::String ^str4 = gcnew System::String(prefsFile->readascii(len));
			ubw->textBox2->Text = str4;
			ubw->openFileDialog2->FileName = str4;
			ubw->checkBox1->Checked = ((bool) prefsFile->readlong());
			System::Runtime::InteropServices::Marshal::FreeHGlobal((System::IntPtr) dir); 
			 }
	private: System::Void menuStrip1_ItemClicked(System::Object^  sender, System::Windows::Forms::ToolStripItemClickedEventArgs^  e) {
			 }
	private: System::Void languageToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 }
private: System::Void deutschToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			fileToolStripMenuItem->Text = "Datei";
			openToolStripMenuItem->Text = "Öffnen";
			saveToolStripMenuItem->Text = "Speichern";
			convertToolStripMenuItem->Text = "Umwandeln";
			languageToolStripMenuItem->Text = "Sprache";
			englishToolStripMenuItem->Text = "Englisch";
			germanToolStripMenuItem->Text = "Deutsch";
			helpToolStripMenuItem->Text = "Hilfe";
			aboutToolStripMenuItem->Text = "Über...";
			toolsToolStripMenuItem->Text = "Extras";
			binToolStripMenuItem->Text = "BIN-Datei erneuern";
			scriptsToolStripMenuItem->Text = "Maxscripts einrichten";
			button3->Text = "Umwandeln";
			groupBox1->Text = "Pfade";
			label1->Text = "Quelldatei";
			label2->Text = "Zieldatei";
			folderBrowserDialog1->Description = "Wählen Sie das Installationsverzeichnis von 3ds Max / gmax.";
			//ubw
			ubw->label1->Text = "BIN-Datei (compiled_meshes.bin)";
			ubw->label2->Text = "XMSH-Datei";
			ubw->Text = "BIN-Datei erneuern";
			ubw->button3->Text = "Weiter";
			ubw->button4->Text = "Abbrechen";
			ubw->groupBox1->Text = "Einstellungen";
			ubw->checkBox1->Text = "BIN-Datei ersetzen";
			output::setlanguage(1);
		 }
private: System::Void englischToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			fileToolStripMenuItem->Text = "File";
			openToolStripMenuItem->Text = "Open";
			saveToolStripMenuItem->Text = "Save";
			convertToolStripMenuItem->Text = "Convert";
			languageToolStripMenuItem->Text = "Language";
			englishToolStripMenuItem->Text = "English";
			germanToolStripMenuItem->Text = "German";
			helpToolStripMenuItem->Text = "Help";
			aboutToolStripMenuItem->Text = "About...";
			toolsToolStripMenuItem->Text = "Tools";
			binToolStripMenuItem->Text = "Update BIN file";
			scriptsToolStripMenuItem->Text = "Set up maxscripts";
			button3->Text = "Convert";
			groupBox1->Text = "Paths";
			label1->Text = "Source File";
			label2->Text = "Target File";
			folderBrowserDialog1->Description = "Choose the 3ds Max / gmax installation directory.";
			//ubw
			ubw->label1->Text = "BIN file (compiled_meshes.bin)";
			ubw->label2->Text = "XMSH file";
			ubw->Text = "Update BIN file";
			ubw->button3->Text = "Continue";
			ubw->button4->Text = "Cancel";
			ubw->groupBox1->Text = "Preferences";
			ubw->checkBox1->Text = "Replace BIN file";
			output::setlanguage(0);
		 }
private: System::Void groupBox1_Enter(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
			 openFileDialog1->ShowDialog();
		 }
private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
			 saveFileDialog1->ShowDialog();
		 }
private: System::Void saveFileDialog1_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {
			textBox2->Text = saveFileDialog1->FileName;
		 }
private: System::Void openFileDialog1_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {
			textBox1->Text = openFileDialog1->FileName;
		 }
private: System::Void aboutToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			//aw->Close();
			//aw = gcnew about();
			aw->Show();
		 }
private: System::Void toolStripMenuItem2_Click(System::Object^  sender, System::EventArgs^  e) {
			this->button3->PerformClick();
		 }
private: System::Void openToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->button1->PerformClick();
		 }
private: System::Void speichernToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 this->button2->PerformClick();
		 }
private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {
			if ((this->textBox1->Text != "") && (this->textBox2->Text != ""))
			{
				System::String ^angle;
				if (this->checkBox1->Checked)
				{
					angle = this->numericUpDown1->Text;
				}
				else
				{
					angle = gcnew System::String("-1");
				}
				System::String ^lang;
				if (output::getlanguage() == 1)
				{
					lang = gcnew System::String("D");
				}
				else
				{
					lang = gcnew System::String("E");
				}
				System::String ^execpath = Application::ExecutablePath;
				System::IO::FileInfo ^fileinfo = gcnew System::IO::FileInfo(execpath);
				System::String ^directory = fileinfo->DirectoryName;
				System::IO::FileInfo ^ofi = gcnew System::IO::FileInfo(this->textBox1->Text);
				System::String ^name;
				if (System::String::Compare(ofi->Extension, ".max", true) == 0)
				{
					name = gcnew System::String("max2xmsh.exe");
				}
				else if (System::String::Compare(ofi->Extension, ".gmax", true) == 0)
				{
					name = gcnew System::String("max2xmsh.exe");
				}
				else if (System::String::Compare(ofi->Extension, ".obj", true) == 0)
				{
					name = gcnew System::String("obj2xmsh.exe");
				}
				else if (System::String::Compare(ofi->Extension, ".ase", true) == 0)
				{
					name = gcnew System::String("ase2xmsh.exe");
				}
				else
				{
					output::error("Ungültiger Dateityp", "Invalid file type");
					return;
				}
				System::String ^args = ("\"" + this->textBox1->Text + "\" \"" + this->textBox2->Text + "\" " + angle + " " + lang);
				//process = gcnew System::Diagnostics::Process;
				System::Diagnostics::ProcessStartInfo ^startinfo = gcnew System::Diagnostics::ProcessStartInfo;
				//startinfo->FileName = directory->
				startinfo->WorkingDirectory = directory;
				startinfo->UseShellExecute = true;
				startinfo->FileName = name;
				startinfo->Arguments = args; 
				process->StartInfo = startinfo;
				process->Start();
				this->deactivate_button();
			}
		 }
private: System::Void scriptsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			if (folderBrowserDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				System::String ^startUpPath;
				System::String ^stdScriptsPath;
				startUpPath = folderBrowserDialog1->SelectedPath + "\\scripts\\startup\\";
				stdScriptsPath = folderBrowserDialog1->SelectedPath + "\\stdplugs\\stdscripts\\";
				System::String ^execpath = Application::ExecutablePath;
				System::IO::FileInfo ^fileinfo = gcnew System::IO::FileInfo(execpath);
				System::String ^directory = fileinfo->DirectoryName;
				System::String ^scriptSourceDir = directory + "\\maxscripts";
				if (output::getlanguage() == 1)
				{
					scriptSourceDir += "\\de\\";
				}
				else
				{
					scriptSourceDir += "\\en\\";
				}
				try
				{					
					System::IO::File::Copy((scriptSourceDir + "RisenToolsStartup.ms"), (startUpPath + "RisenToolsStartup.ms"), true);
					System::IO::File::Copy((scriptSourceDir + "runImpXmsh.ms"), (stdScriptsPath + "runImpXmsh.ms"), true);
					System::IO::File::Copy((scriptSourceDir + "runPrepScene.ms"), (stdScriptsPath + "runPrepScene.ms"), true);
					System::IO::File::Copy((scriptSourceDir + "runRenewMeshes.ms"), (stdScriptsPath + "runRenewMeshes.ms"), true);
					char *path = (char *)(void *)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(startUpPath + "RisenToolsStartup.ms");
					file s(path, 3);
					System::Random ^myrand = gcnew System::Random();
					int rint = myrand->Next();
					System::String ^rstr = gcnew System::String("");
					rstr = rint.ToString("x");
					char *rchar = (char *)(void *)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(rstr);
					s.jump(49);
					s.writechararray(rchar, rstr->Length);
					System::Runtime::InteropServices::Marshal::FreeHGlobal((System::IntPtr) rchar);
					System::Runtime::InteropServices::Marshal::FreeHGlobal((System::IntPtr) path);
					output::msg("Die Dateien wurden erfolgreich kopiert.", "The files have been copied succesfully");
				}
				catch (Exception ^)
				{
					output::error("Die Dateien konnten nicht kopiert werden. Überprüfen Sie Ihren angegebenen Pfad", "The files couldn't be copied. Check your specified path.");
				}
			}
		 }
private: System::Void extrasToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void process_exit(System::Object^  sender, System::EventArgs^  e) {
			System::EventHandler ^eh = gcnew System::EventHandler(this, &Form1::activate_button);
			this->Invoke(eh);
			if ((this->process->ExitCode == 0) && (this->process->StartInfo->FileName != "edit_comp_meshes.exe"))
			{
				ubw->textBox2->Text = this->textBox2->Text;
				ubw->Show();
			}
			else if ((this->process->ExitCode == 0) && (this->process->StartInfo->FileName == "edit_comp_meshes.exe"))
			{
				ubw->Hide();
			}
		 }
private: System::Void activate_button(System::Object^  sender, System::EventArgs^  e) { 
			this->button3->Enabled = true;
			ubw->button3->Enabled = true;
		 }
public: System::Void deactivate_button(System::Void) { 
			this->button3->Enabled = false;
			ubw->button3->Enabled = false;
		 }
private: System::Void ubw_button3_Click(System::Object^  sender, System::EventArgs^  e) { 
			if ((ubw->textBox1->Text != "") && (ubw->textBox2->Text != ""))
			{
				System::String ^lang;
				if (output::getlanguage() == 1)
				{
					lang = gcnew System::String("D");
				}
				else
				{
					lang = gcnew System::String("E");
				}
				System::String ^replace;
				if (ubw->checkBox1->Checked == true)
				{
					replace = gcnew System::String("Y");
				}
				else
				{
					replace = gcnew System::String("N");
				}
				System::String ^execpath = Application::ExecutablePath;
				System::IO::FileInfo ^fileinfo = gcnew System::IO::FileInfo(execpath);
				System::String ^directory = fileinfo->DirectoryName;
				System::String ^name = gcnew System::String("edit_comp_meshes.exe");
				System::String ^args = ("\"" + ubw->textBox2->Text + "\" \"" + ubw->textBox1->Text + "\" " + replace + " " + lang);
				//process = gcnew System::Diagnostics::Process;
				System::Diagnostics::ProcessStartInfo ^startinfo = gcnew System::Diagnostics::ProcessStartInfo;
				//startinfo->FileName = directory->
				startinfo->WorkingDirectory = directory;
				startinfo->UseShellExecute = true;
				startinfo->FileName = name;
				startinfo->Arguments = args; 
				process->StartInfo = startinfo;
				process->Start();
				this->deactivate_button();
			}
		 }
private: System::Void compiledmeshesbinErweiternToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			//ubw->Close();
			//ubw = gcnew update_bin();
			/*
			if (output::getlanguage() == 1)
			{
				ubw->Text = "BIN-Datei erneuern";
			}
			else
			{
				ubw->Text = "Update BIN file";
			}
			*/
			ubw->Show();
		 }
private: System::Void numericUpDown1_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void checkBox1_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			this->numericUpDown1->Enabled = checkBox1->Checked;
		 }
};
}

