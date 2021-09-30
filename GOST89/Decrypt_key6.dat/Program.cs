using System;
using System.Security.Cryptography;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Formats.Asn1;
using GOST89;

namespace Decrypt_key6.dat
{
    class Program
    {
        static void Main(string[] args)
        {
            var filecontent = File.ReadAllBytes(@"key-6.dat");
            ASN1Decode(filecontent);
            Console.ReadLine();
        }

        static void ASN1Decode(byte[] datastream)
        {
            AsnReader reader = new AsnReader(datastream, AsnEncodingRules.DER);

            var mainseq = reader.ReadSequence();
            var macpadseq = mainseq.ReadSequence();
            //var oid = macpadseq.ReadObjectIdentifier();
            var macpadseq2 = macpadseq.ReadSequence();
            //var mac = macpadseq2.ReadOctetString();
            var pad = macpadseq2.ReadOctetString();
            var cryptedkey = mainseq.ReadOctetString();

            var paddeddata = new byte[cryptedkey.Length + pad.Length];
            cryptedkey.CopyTo(paddeddata, 0);
            pad.CopyTo(paddeddata, cryptedkey.Length);
            File.WriteAllBytes("cryptkey.txt", paddeddata);
            var plainkey = Compat.DecodeData(cryptedkey.Length, paddeddata, Encoding.UTF8.GetBytes("tect4"));
            ASN1DecodePK(plainkey);
        }

        static void ASN1DecodePK(byte[] datastream)
        {
            //var rr = BitConverter.ToUInt32(new byte[4] { 0xb6, 0x0f, 0xd2, 0xd8 });
            AsnReader reader = new AsnReader(datastream, AsnEncodingRules.DER);
            byte[] encodedRequestInfo = reader.PeekEncodedValue().ToArray();
            var pkinfo = reader.ReadSequence();
            var version = pkinfo.ReadInteger();
            Console.WriteLine($"version: {version}");
            var priv0_obj = pkinfo.ReadSequence();
            var priv_oid = priv0_obj.ReadObjectIdentifier();
            Console.WriteLine($"OID: {priv_oid}");
            var priv_p_object = priv0_obj.ReadSequence();
            var curve_obj = priv_p_object.ReadSequence();
            var sbox = priv_p_object.ReadOctetString();
            var ss = new AsnEncodedData(sbox);
            Console.WriteLine($"sbox ({ss.RawData.Length}): {ss.Format(true)}");
            //var curve_oid = curve_obj.ReadObjectIdentifier();
            var curve_param_obj = curve_obj.ReadSequence();
            //var curve_p_obj = curve_obj.ReadSequence();
            var param_m = curve_param_obj.ReadInteger();
            Console.WriteLine($"param_m: {param_m}");
            var tag = curve_param_obj.PeekTag();
            switch ((UniversalTagNumber)tag.TagValue)
            {
                case UniversalTagNumber.Integer:
                    var curve_p_trinominal = curve_param_obj.ReadInteger();
                    Console.WriteLine($"trinominal: {curve_p_trinominal}");
                    break;
                case UniversalTagNumber.Sequence:
                    var curve_pentanominal_obj = curve_param_obj.ReadSequence();
                    var k1 = curve_pentanominal_obj.ReadInteger();
                    var k2 = curve_pentanominal_obj.ReadInteger();
                    var k3 = curve_pentanominal_obj.ReadInteger();
                    Console.WriteLine($"pentanominal k1: {k1}");
                    Console.WriteLine($"pentanominal k2: {k2}");
                    Console.WriteLine($"pentanominal k3: {k3}");
                    break;
                default:
                    var octss = curve_param_obj.PeekEncodedValue().ToArray();
                    curve_param_obj.ReadOctetString();
                    ss = new AsnEncodedData(octss);
                    Console.WriteLine($"unknown ({ss.RawData.Length}): {ss.Format(true)}");
                    break;
            }
            var param_a = curve_obj.ReadInteger();
            Console.WriteLine($"param_a: {param_a}");
            var param_b = curve_obj.ReadOctetString();
            ss = new AsnEncodedData(param_b);
            Console.WriteLine($"param_b ({ss.RawData.Length}): {ss.Format(true)}");
            var orderbytes = curve_obj.ReadInteger();
            var ordb = orderbytes.ToByteArray();
            var tmplist = new List<UInt32>();
            var tmpob = orderbytes;
            var tmplistarr = new List<byte>();
            while (tmpob > 0)
            {
                var newitem = (UInt32)(tmpob & 0xffffffff);
                tmplist.Add(newitem);
                tmplistarr.AddRange(BitConverter.GetBytes(newitem));
                tmpob >>= 32;
            }
            var aa = tmplistarr.ToArray();
            var sa = new AsnEncodedData(ordb).Format(false);
            Console.WriteLine($"order: {orderbytes}");
            var bp = curve_obj.ReadOctetString();
            ss = new AsnEncodedData(bp);
            Console.WriteLine($"bp ({ss.RawData.Length}): {ss.Format(true)}");
            var param_d = pkinfo.ReadOctetString();
            ss = new AsnEncodedData(param_d);
            Console.WriteLine($"param_d ({ss.RawData.Length}): {ss.Format(true)}");
            tag = pkinfo.PeekTag();
            var keyattrset = pkinfo.ReadSequence(tag);
            while (keyattrset.HasData)
            {
                Console.WriteLine($"");
                var key_attr = keyattrset.ReadSequence();
                var attr1oid = key_attr.ReadObjectIdentifier();
                Console.WriteLine($"Key Attribute OID: {attr1oid}");
                var attrvaluesset = key_attr.ReadSetOf();
                Console.WriteLine($"Key Attribute values:");
                while (attrvaluesset.HasData)
                {
                    var attrsettag = attrvaluesset.PeekTag();
                    switch ((UniversalTagNumber)attrsettag.TagValue)
                    {
                        case UniversalTagNumber.BitString:
                            var attrparam_d = attrvaluesset.ReadBitString(out int unusedcpunt);
                            var pdbytes = new AsnEncodedData(attrparam_d);
                            Console.WriteLine($"param_d ({pdbytes.RawData.Length}): {pdbytes.Format(true)}, unused: {unusedcpunt}");
                            break;
                        case UniversalTagNumber.Sequence:
                            var dstuparams = attrvaluesset.ReadSequence();
                            attrsettag = dstuparams.PeekTag();
                            switch ((UniversalTagNumber)attrsettag.TagValue)
                            {
                                case UniversalTagNumber.ObjectIdentifier:
                                    var curve_oid = dstuparams.ReadObjectIdentifier();
                                    Console.WriteLine($"Curve OID: {curve_oid}");
                                    break;
                                case UniversalTagNumber.Sequence:
                                    var curveparams = dstuparams.ReadSequence();
                                    Console.WriteLine($"Curve parameters:");
                                    var curve_p = curveparams.ReadSequence();
                                    var curveparam_m = curve_p.ReadInteger();
                                    Console.WriteLine($"param_m: {curveparam_m}");
                                    attrsettag = curve_p.PeekTag();
                                    switch ((UniversalTagNumber)attrsettag.TagValue)
                                    {
                                        case UniversalTagNumber.Integer:
                                            var curve_p_trinominal = curve_p.ReadInteger();
                                            Console.WriteLine($"trinominal value: {curve_p_trinominal}");
                                            break;
                                        case UniversalTagNumber.Sequence:
                                            var curve_pentanominal_obj = curve_p.ReadSequence();
                                            var k1 = curve_pentanominal_obj.ReadInteger();
                                            var k2 = curve_pentanominal_obj.ReadInteger();
                                            var k3 = curve_pentanominal_obj.ReadInteger();
                                            Console.WriteLine($"pentanominal k1: {k1}");
                                            Console.WriteLine($"pentanominal k2: {k2}");
                                            Console.WriteLine($"pentanominal k3: {k3}");
                                            break;
                                        default:
                                            break;
                                    }
                                    var attr_param_a = curveparams.ReadInteger();
                                    Console.WriteLine($"param_a: {attr_param_a}");
                                    var attr_param_b = curveparams.ReadOctetString();
                                    ss = new AsnEncodedData(attr_param_b);
                                    Console.WriteLine($"param_b ({ss.RawData.Length}): {ss.Format(true)}");
                                    var attr_orderbytes = curveparams.ReadInteger();
                                    Console.WriteLine($"order: {attr_orderbytes}");
                                    var attr_bp = curveparams.ReadOctetString();
                                    ss = new AsnEncodedData(attr_bp);
                                    Console.WriteLine($"bp ({ss.RawData.Length}): {ss.Format(true)}");
                                    break;
                                case UniversalTagNumber.OctetString:
                                    var octss = dstuparams.ReadOctetString();
                                    ss = new AsnEncodedData(octss);
                                    Console.WriteLine($"byte[{ss.RawData.Length}]: {ss.Format(true)}");
                                    break;
                                default:
                                    octss = dstuparams.PeekEncodedValue().ToArray();
                                    ss = new AsnEncodedData(octss);
                                    Console.WriteLine($"unknown ({ss.RawData.Length}): {ss.Format(true)}");
                                    break;
                            }
                            if (dstuparams.HasData)
                            {
                                var dke = dstuparams.ReadOctetString();
                                ss = new AsnEncodedData(dke);
                                Console.WriteLine($"dke ({ss.RawData.Length}): {ss.Format(true)}");
                                if (dstuparams.HasData)
                                {
                                    var dke2 = dstuparams.ReadOctetString();
                                    ss = new AsnEncodedData(dke2);
                                    Console.WriteLine($"dke2 ({ss.RawData.Length}): {ss.Format(true)}");
                                }
                            }
                            break;
                        case UniversalTagNumber.OctetString:
                            var octs = attrvaluesset.PeekEncodedValue().ToArray();
                            attrvaluesset.ReadOctetString();
                            ss = new AsnEncodedData(octs);
                            Console.WriteLine($"byte[{ss.RawData.Length}]: {ss.Format(true)}");
                            break;
                        default:
                            var octsss = attrvaluesset.PeekEncodedValue().ToArray();
                            ss = new AsnEncodedData(octsss);
                            Console.WriteLine($"unknown ({ss.RawData.Length}): {ss.Format(true)}");
                            break;
                    }
                }
            }
        }
    }
}
