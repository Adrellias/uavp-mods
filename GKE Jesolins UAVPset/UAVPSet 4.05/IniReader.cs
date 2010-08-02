// UAVPSet
// Copyright (C) 2007  Thorsten Raab
// Email: thorsten.raab@gmx.at
// English Text and Graphics modified April 2009 by Jim Solinski
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using System.IO;

namespace UAVP.UAVPSet
{
    /// <summary>
    /// diese klasse wurde teilweise aus einem Forum übernommen!!!
    /// </summary>
    class IniReader
    {
        private inidatei[] inizeilen;
        private string datei = "";

        public IniReader(string file)
        {
            datei = file;
            string abschnitt = "";
            string[] iniread = File.ReadAllLines(file);
            inizeilen = new inidatei[iniread.Length];
            Regex reg1 = new Regex(@"^(?<befehl>([^=])+)=(?<wert>([^=#])+)(#(?<kommentar>.*))?$");
            Regex reg2 = new Regex(@"^[^=]*\[(?<abschnitt>([^\]]*))\][^=]*$");
            for (int i = 0; i < inizeilen.Length; i++) 
            {
                Match m1 = reg1.Match(iniread[i]);
                Match m2 = reg2.Match(iniread[i]);
                if (m1.Success) 
                {
                    inizeilen[i].befehl = m1.Groups["befehl"].Value.Trim();
                    inizeilen[i].wert = m1.Groups["wert"].Value.Trim();
                    inizeilen[i].kommentar = m1.Groups["kommentar"].Value.Trim();
                    inizeilen[i].abschnitt = abschnitt;
                    inizeilen[i].text = "";
                } 
                else 
                {
                    if (m2.Success) 
                    {
                        abschnitt = m2.Groups["abschnitt"].Value.Trim();
                    }
                    inizeilen[i].befehl = "";
                    inizeilen[i].wert = "";
                    inizeilen[i].kommentar = "";
                    inizeilen[i].abschnitt = "";
                    inizeilen[i].text = iniread[i];
                }
            }
        }

        private struct inidatei
        {
            public string befehl;
            public string wert;
            public string kommentar;
            public string abschnitt;
            public string text;
        }

        //Verwenden der ParameterSets.Struc
        //public struct Return
        //{
        //    public string Command;
        //    public string Value;
        //    public string Comment;
        //    public string Chapter;
        //}

        public ParameterSets.ParameterSetsStruc Get(string befehl)
        {
            ParameterSets.ParameterSetsStruc rueckgabe = new ParameterSets.ParameterSetsStruc();
            rueckgabe.Command = befehl;
            rueckgabe.Value = "";
            rueckgabe.Comment = "";
            foreach (inidatei i in inizeilen) 
            {
                if (i.befehl == befehl) 
                {
                    rueckgabe.Value = i.wert;
                    rueckgabe.Comment = i.kommentar;
                    rueckgabe.Chapter = i.abschnitt;
                }
            }
            return rueckgabe;
        }

        public ParameterSets.ParameterSetsStruc[] GetChapter(string chapter)
        {
            ParameterSets.ParameterSetsStruc[] rueckgabe = new ParameterSets.ParameterSetsStruc[0];
            foreach (inidatei i in inizeilen) 
            {
                if (i.abschnitt == chapter) 
                {
                    Array.Resize(ref rueckgabe, rueckgabe.Length + 1);
                    rueckgabe[rueckgabe.Length - 1].Chapter = chapter;
                    rueckgabe[rueckgabe.Length - 1].Command = i.befehl;
                    rueckgabe[rueckgabe.Length - 1].Comment = i.kommentar;
                    rueckgabe[rueckgabe.Length - 1].Value = i.wert;
                }
            }
            return rueckgabe;
        }

        public bool Set(string befehl, string wert)
        {
            bool success = false;
            for (int i = 0; i < inizeilen.Length; i++) 
            {
                if (inizeilen[i].befehl == befehl) 
                {
                    inizeilen[i].wert = wert;
                    success = true;
                }
            }
            if (!success) 
            {
                return success;
            }
            StreamWriter writer = new StreamWriter(datei);
            for (int i = 0; i < inizeilen.Length; i++) 
            {
                if (!String.IsNullOrEmpty(inizeilen[i].befehl)) 
                {
                    writer.Write(inizeilen[i].befehl + "=" + inizeilen[i].wert);
                    if (!String.IsNullOrEmpty(inizeilen[i].kommentar)) 
                    {
                        writer.Write(" #" + inizeilen[i].kommentar);
                    }
                    if (i != (inizeilen.Length - 1)) 
                    {
                        writer.WriteLine();
                    }
                } 
                else 
                {
                    writer.Write(inizeilen[i].text);
                    if (i != (inizeilen.Length - 1)) 
                    {
                        writer.WriteLine();
                    }
                }
            }
            writer.Close();
            return success;
        }
    }
}
