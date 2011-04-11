#pragma once


namespace RisenMapPosition {

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
	private: System::Windows::Forms::Label^  label1;








	private: System::Windows::Forms::Label^  XLabel;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::NumericUpDown^  RZUD;




	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::NumericUpDown^  RXUD;




	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::NumericUpDown^  MYUD;


	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::NumericUpDown^  MXUD;



	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::GroupBox^  Info;
	private: System::Windows::Forms::LinkLabel^  linkLabel1;
	private: System::Windows::Forms::Label^  label12;

	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::NumericUpDown^  YXUD;
	private: System::Windows::Forms::NumericUpDown^  YZUD;




	private: System::Windows::Forms::Label^  label13;
	private: System::Windows::Forms::Label^  label11;
	private: System::Windows::Forms::Label^  label14;
	private: System::Windows::Forms::Label^  label15;






	protected: 

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
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->XLabel = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->RZUD = (gcnew System::Windows::Forms::NumericUpDown());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->RXUD = (gcnew System::Windows::Forms::NumericUpDown());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->MYUD = (gcnew System::Windows::Forms::NumericUpDown());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->MXUD = (gcnew System::Windows::Forms::NumericUpDown());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->Info = (gcnew System::Windows::Forms::GroupBox());
			this->linkLabel1 = (gcnew System::Windows::Forms::LinkLabel());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->YZUD = (gcnew System::Windows::Forms::NumericUpDown());
			this->YXUD = (gcnew System::Windows::Forms::NumericUpDown());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->groupBox1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->RZUD))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->RXUD))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MYUD))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MXUD))->BeginInit();
			this->groupBox2->SuspendLayout();
			this->Info->SuspendLayout();
			this->groupBox3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->YZUD))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->YXUD))->BeginInit();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"label1.Image")));
			this->label1->ImageAlign = System::Drawing::ContentAlignment::TopLeft;
			this->label1->Location = System::Drawing::Point(12, 10);
			this->label1->MinimumSize = System::Drawing::Size(640, 512);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(640, 512);
			this->label1->TabIndex = 0;
			this->label1->Click += gcnew System::EventHandler(this, &Form1::label1_Click);
			// 
			// XLabel
			// 
			this->XLabel->AutoSize = true;
			this->XLabel->Location = System::Drawing::Point(36, 34);
			this->XLabel->Name = L"XLabel";
			this->XLabel->Size = System::Drawing::Size(14, 13);
			this->XLabel->TabIndex = 8;
			this->XLabel->Text = L"X";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->label11);
			this->groupBox1->Controls->Add(this->RZUD);
			this->groupBox1->Controls->Add(this->label6);
			this->groupBox1->Controls->Add(this->RXUD);
			this->groupBox1->Controls->Add(this->label5);
			this->groupBox1->Controls->Add(this->label4);
			this->groupBox1->Location = System::Drawing::Point(658, 9);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(210, 146);
			this->groupBox1->TabIndex = 9;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Risen/Cinema 4D (left-handed)";
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->label11->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"label11.Image")));
			this->label11->Location = System::Drawing::Point(135, 68);
			this->label11->MinimumSize = System::Drawing::Size(68, 68);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(68, 68);
			this->label11->TabIndex = 13;
			// 
			// RZUD
			// 
			this->RZUD->Location = System::Drawing::Point(149, 42);
			this->RZUD->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {54800, 0, 0, 0});
			this->RZUD->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {36800, 0, 0, System::Int32::MinValue});
			this->RZUD->Name = L"RZUD";
			this->RZUD->Size = System::Drawing::Size(54, 20);
			this->RZUD->TabIndex = 11;
			this->RZUD->ValueChanged += gcnew System::EventHandler(this, &Form1::RZUD_ValueChanged);
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(129, 44);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(20, 13);
			this->label6->TabIndex = 12;
			this->label6->Text = L"Z: ";
			// 
			// RXUD
			// 
			this->RXUD->Location = System::Drawing::Point(149, 16);
			this->RXUD->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {55600, 0, 0, 0});
			this->RXUD->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {58900, 0, 0, System::Int32::MinValue});
			this->RXUD->Name = L"RXUD";
			this->RXUD->Size = System::Drawing::Size(54, 20);
			this->RXUD->TabIndex = 10;
			this->RXUD->ValueChanged += gcnew System::EventHandler(this, &Form1::RXUD_ValueChanged);
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(129, 18);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(20, 13);
			this->label5->TabIndex = 10;
			this->label5->Text = L"X: ";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->label4->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"label4.Image")));
			this->label4->Location = System::Drawing::Point(6, 16);
			this->label4->MinimumSize = System::Drawing::Size(120, 120);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(120, 120);
			this->label4->TabIndex = 0;
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(129, 46);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(20, 13);
			this->label7->TabIndex = 12;
			this->label7->Text = L"Y: ";
			// 
			// MYUD
			// 
			this->MYUD->Location = System::Drawing::Point(149, 44);
			this->MYUD->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {54800, 0, 0, 0});
			this->MYUD->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {36800, 0, 0, System::Int32::MinValue});
			this->MYUD->Name = L"MYUD";
			this->MYUD->Size = System::Drawing::Size(54, 20);
			this->MYUD->TabIndex = 10;
			this->MYUD->ValueChanged += gcnew System::EventHandler(this, &Form1::MYUD_ValueChanged);
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(129, 20);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(20, 13);
			this->label8->TabIndex = 10;
			this->label8->Text = L"X: ";
			// 
			// MXUD
			// 
			this->MXUD->Location = System::Drawing::Point(149, 18);
			this->MXUD->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {55600, 0, 0, 0});
			this->MXUD->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {58900, 0, 0, System::Int32::MinValue});
			this->MXUD->Name = L"MXUD";
			this->MXUD->Size = System::Drawing::Size(54, 20);
			this->MXUD->TabIndex = 11;
			this->MXUD->ValueChanged += gcnew System::EventHandler(this, &Form1::MXUD_ValueChanged);
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->label9->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"label9.Image")));
			this->label9->Location = System::Drawing::Point(6, 18);
			this->label9->MinimumSize = System::Drawing::Size(120, 120);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(120, 120);
			this->label9->TabIndex = 0;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->label14);
			this->groupBox2->Controls->Add(this->MYUD);
			this->groupBox2->Controls->Add(this->MXUD);
			this->groupBox2->Controls->Add(this->label7);
			this->groupBox2->Controls->Add(this->label9);
			this->groupBox2->Controls->Add(this->label8);
			this->groupBox2->Location = System::Drawing::Point(658, 161);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(210, 148);
			this->groupBox2->TabIndex = 10;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"3ds Max/GMax/Blender (right-handed)";
			// 
			// label14
			// 
			this->label14->AutoSize = true;
			this->label14->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->label14->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"label14.Image")));
			this->label14->Location = System::Drawing::Point(135, 70);
			this->label14->MinimumSize = System::Drawing::Size(68, 68);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(68, 68);
			this->label14->TabIndex = 14;
			// 
			// Info
			// 
			this->Info->Controls->Add(this->linkLabel1);
			this->Info->Controls->Add(this->label12);
			this->Info->Controls->Add(this->label10);
			this->Info->Location = System::Drawing::Point(658, 465);
			this->Info->Name = L"Info";
			this->Info->Size = System::Drawing::Size(210, 56);
			this->Info->TabIndex = 11;
			this->Info->TabStop = false;
			this->Info->Text = L"Info";
			// 
			// linkLabel1
			// 
			this->linkLabel1->AutoSize = true;
			this->linkLabel1->Location = System::Drawing::Point(68, 29);
			this->linkLabel1->Name = L"linkLabel1";
			this->linkLabel1->Size = System::Drawing::Size(108, 13);
			this->linkLabel1->TabIndex = 12;
			this->linkLabel1->TabStop = true;
			this->linkLabel1->Text = L"baltram-lielb@web.de";
			this->linkLabel1->LinkClicked += gcnew System::Windows::Forms::LinkLabelLinkClickedEventHandler(this, &Form1::linkLabel1_LinkClicked);
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Location = System::Drawing::Point(6, 29);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(56, 13);
			this->label12->TabIndex = 12;
			this->label12->Text = L"by Baltram";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(6, 16);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(117, 13);
			this->label10->TabIndex = 0;
			this->label10->Text = L"Risen Map Viewer v0.3";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(129, 44);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(20, 13);
			this->label2->TabIndex = 12;
			this->label2->Text = L"Z: ";
			this->label2->Click += gcnew System::EventHandler(this, &Form1::label2_Click);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->label3->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"label3.Image")));
			this->label3->Location = System::Drawing::Point(6, 16);
			this->label3->MinimumSize = System::Drawing::Size(120, 120);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(120, 120);
			this->label3->TabIndex = 0;
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->label15);
			this->groupBox3->Controls->Add(this->YZUD);
			this->groupBox3->Controls->Add(this->YXUD);
			this->groupBox3->Controls->Add(this->label2);
			this->groupBox3->Controls->Add(this->label3);
			this->groupBox3->Controls->Add(this->label13);
			this->groupBox3->Location = System::Drawing::Point(658, 315);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(210, 144);
			this->groupBox3->TabIndex = 13;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Maya (right-handed)";
			// 
			// label15
			// 
			this->label15->AutoSize = true;
			this->label15->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->label15->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"label15.Image")));
			this->label15->Location = System::Drawing::Point(135, 68);
			this->label15->MinimumSize = System::Drawing::Size(68, 68);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(68, 68);
			this->label15->TabIndex = 14;
			// 
			// YZUD
			// 
			this->YZUD->Location = System::Drawing::Point(149, 42);
			this->YZUD->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {36800, 0, 0, 0});
			this->YZUD->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {54800, 0, 0, System::Int32::MinValue});
			this->YZUD->Name = L"YZUD";
			this->YZUD->Size = System::Drawing::Size(54, 20);
			this->YZUD->TabIndex = 10;
			this->YZUD->ValueChanged += gcnew System::EventHandler(this, &Form1::YZUD_ValueChanged);
			// 
			// YXUD
			// 
			this->YXUD->Location = System::Drawing::Point(149, 16);
			this->YXUD->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {55600, 0, 0, 0});
			this->YXUD->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {58900, 0, 0, System::Int32::MinValue});
			this->YXUD->Name = L"YXUD";
			this->YXUD->Size = System::Drawing::Size(54, 20);
			this->YXUD->TabIndex = 11;
			this->YXUD->ValueChanged += gcnew System::EventHandler(this, &Form1::YXUD_ValueChanged);
			// 
			// label13
			// 
			this->label13->AutoSize = true;
			this->label13->Location = System::Drawing::Point(129, 18);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(20, 13);
			this->label13->TabIndex = 10;
			this->label13->Text = L"X: ";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(875, 531);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->Info);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->XLabel);
			this->Controls->Add(this->label1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->MaximizeBox = false;
			this->Name = L"Form1";
			this->Text = L"Risen Map Position v0.3";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->RZUD))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->RXUD))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MYUD))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->MXUD))->EndInit();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->Info->ResumeLayout(false);
			this->Info->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->YZUD))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->YXUD))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
			 }
	private: System::Void label1_Click(System::Object^  sender, System::EventArgs^  e) {
				System::Drawing::Point cursorPosLocal = label1->PointToClient(Control::MousePosition);
				double x = cursorPosLocal.X;
				double y = cursorPosLocal.Y;

				RXUD->Value = static_cast<int>(x * 175.5 - 58696);
				RZUD->Value = static_cast<int>(y * (-175.5) + 54376);

				MXUD->Value = RXUD->Value;
				MYUD->Value = RZUD->Value;

				YXUD->Value = RXUD->Value;
				YZUD->Value = RZUD->Value * (-1);
			 }
private: System::Void linkLabel1_LinkClicked(System::Object^  sender, System::Windows::Forms::LinkLabelLinkClickedEventArgs^  e) {
			 System::Diagnostics::Process::Start("mailto:baltram-lielb@web.de");
		 }
private: System::Void RXUD_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			if (MXUD->Value != RXUD->Value) {
				MXUD->Value = RXUD->Value;
			}
			if (YXUD->Value != RXUD->Value) {
				YXUD->Value = RXUD->Value;
			}
			System::Drawing::Point point;
			point.X = ((static_cast<int>(RXUD->Value) + 58696) / 175.5) - 6;
			point.Y = ((static_cast<int>(RZUD->Value) - 54376) / (-175.5)) - 6;
			XLabel->Location = this->PointToClient(label1->PointToScreen(point));
		 }
private: System::Void RZUD_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			if (MYUD->Value != RZUD->Value) {
				MYUD->Value = RZUD->Value;
			}
			if (YZUD->Value != (RZUD->Value * (-1))) {
				YZUD->Value = (RZUD->Value * (-1));
			}
			System::Drawing::Point point;
			point.X = ((static_cast<int>(RXUD->Value) + 58696) / 175.5) - 6;
			point.Y = ((static_cast<int>(RZUD->Value) - 54376) / (-175.5)) - 6;
			XLabel->Location = this->PointToClient(label1->PointToScreen(point));
		 }
private: System::Void MXUD_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			if (RXUD->Value != MXUD->Value) {
				RXUD->Value = MXUD->Value;
			}
			if (YXUD->Value != MXUD->Value) {
				YXUD->Value = MXUD->Value;
			}
			 //RZUD->Value = MXUD->Value;
		 }
private: System::Void MYUD_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			if (RZUD->Value != MYUD->Value) {
				RZUD->Value = MYUD->Value;
			}
			if (YZUD->Value != (MYUD->Value * (-1))) {
				YZUD->Value = (MYUD->Value * (-1));
			}
		 }
private: System::Void YXUD_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			if (RXUD->Value != YXUD->Value) {
				RXUD->Value = YXUD->Value;
			}
			if (MXUD->Value != YXUD->Value) {
				MXUD->Value = YXUD->Value;
			}
		 }
private: System::Void YZUD_ValueChanged(System::Object^  sender, System::EventArgs^  e) {
			if (RZUD->Value != (YZUD->Value * (-1))) {
				RZUD->Value = (YZUD->Value * (-1));
			}
			if (MYUD->Value != (YZUD->Value * (-1))) {
				MYUD->Value = (YZUD->Value * (-1));
			}
		 }
private: System::Void label2_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
};
}

