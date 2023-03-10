// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace Imgdoc2cmd
{
    using ImgDoc2Net;
    using ImgDoc2Net.Implementation;
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using ImgDoc2Net.Interfaces;

    internal partial class CommandHelper : IDisposable
    {
        private Document? document;

        private IRead2d? reader2d;

        public Document Document
        {
            get
            {
                return this.document ?? throw new InvalidOperationException("no document"); ;
            }
            set
            {
                this.document = value;
            }
        }

        public IRead2d GetRead2d()
        {
            if (this.reader2d == null)
            {
                this.reader2d = this.Document.Get2dReader();
            }

            return this.reader2d;
        }

        public void OpenSourceDocument(string filename)
        {
            using var openExistingOptions = new OpenExistingOptions() { Filename = filename };

            this.document = Document.OpenExisting(openExistingOptions);
        }
    }

    internal partial class CommandHelper
    {
        public void Dispose()
        {
            this.document?.Dispose();
        }
    }
}
