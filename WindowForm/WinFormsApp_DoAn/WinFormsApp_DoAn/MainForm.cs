using Emgu.CV;
using Emgu.CV.Structure;
using Tesseract;


namespace WinFormsApp_DoAn
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
            this.Hide();
            Form1 a = new Form1();
            a.ShowDialog();
        }
        //Dinh nghia
        List<Image<Bgr, Byte>> PlateImagesList = new List<Image<Bgr, Byte>>();
        List<string> PlateNamesList = new List<string>();
        List<Rectangle> listRect = new List<Rectangle>();
        PictureBox[] box = new PictureBox[12];

        //Function of Tesseract
        public TesseractProcessor full_tesseract = null;

        private void MainForm_Load(object sender, EventArgs e)
        {

        }
    }
}
