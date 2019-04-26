﻿// Copyright © 2018 Paddy Xu
// 
// This file is part of QuickLook program.
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

using System;
using System.Drawing;
using System.Windows.Media.Imaging;
using PdfiumViewer;
using QuickLook.Common.ExtensionMethods;
using QuickLook.Common.Helpers;

namespace QuickLook.Plugin.PDFViewer
{
    internal static class PdfPageExtension
    {
        private static int _renderCount;
        private static readonly object LockObj = new object();

        public static BitmapSource RenderThumbnail(this PdfDocumentWrapper doc, int page)
        {
            lock (LockObj)
            {
                if (_renderCount++ == 50)
                {
                    doc.Refresh();
                    _renderCount = 0;
                }
            }

            var size = doc.PdfDocument.PageSizes[page];
            var factorX = 60d / size.Width;
            var factorY = 120d / size.Height;

            return doc.Render(page, Math.Min(factorX, factorY), false);
        }

        public static BitmapSource Render(this PdfDocumentWrapper doc, int page, double factor, bool fixDpi = true)
        {
            var scale = DpiHelper.GetCurrentScaleFactor();
            var dpiX = fixDpi ? scale.Horizontal * DpiHelper.DefaultDpi : 96;
            var dpiY = fixDpi ? scale.Vertical * DpiHelper.DefaultDpi : 96;

            Bitmap bitmap;

            lock (LockObj)
            {
                var size = doc.PdfDocument.PageSizes[page];
                var realWidth = (int) Math.Round(size.Width * scale.Horizontal * factor);
                var realHeight = (int) Math.Round(size.Height * scale.Vertical * factor);

                bitmap = doc.PdfDocument.Render(page, realWidth, realHeight, dpiX, dpiY,
                    PdfRenderFlags.LimitImageCacheSize | PdfRenderFlags.LcdText | PdfRenderFlags.Annotations |
                    PdfRenderFlags.ForPrinting) as Bitmap;
            }

            var bs = bitmap?.ToBitmapSource();
            bitmap?.Dispose();

            bs?.Freeze();
            return bs;
        }
    }
}