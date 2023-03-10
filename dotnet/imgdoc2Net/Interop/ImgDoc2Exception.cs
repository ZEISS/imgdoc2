// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

namespace ImgDoc2Net.Interop
{
    using System;
    using System.Collections.Generic;
    using System.Text;

    [Serializable]
    public class ImgDoc2Exception : Exception
    {
        private int imgdoc2ErrorCode;

        public ImgDoc2Exception() 
        {
        }

        public ImgDoc2Exception(int errorCode, string text)
            : base(text)
        {
            this.imgdoc2ErrorCode = errorCode;
        }

        public ImgDoc2Exception(string message) 
            : base(message)
        {
        }

        public ImgDoc2Exception(string message, Exception innerException) 
            : base(message, innerException)
        {
        }

        public int ImgDoc2ErrorCode
        {
            get { return this.imgdoc2ErrorCode; }
        }
    }
}
