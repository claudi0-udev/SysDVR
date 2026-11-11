using System.Runtime.InteropServices;

namespace SysDVR.Client.Targets.Player
{
    internal static class FsrNative
    {
        [DllImport("cimgui", EntryPoint = "fsr_is_supported")]
        public static extern bool IsSupported();

        [DllImport("cimgui", EntryPoint = "fsr_init")]
        public static extern bool Init(nint sdlRenderer, int srcW, int srcH);

        [DllImport("cimgui", EntryPoint = "fsr_render")]
        public static extern bool Render(
            nint sdlRenderer,
            nint sdlTexture,
            int srcW, int srcH,
            int dstX, int dstY, int dstW, int dstH,
            float sharpness
        );

        [DllImport("cimgui", EntryPoint = "fsr_destroy")]
        public static extern void Destroy();
    }
}
