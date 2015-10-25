package ru.caustic.lasertag.controller_tests;

import junit.framework.Assert;
import junit.framework.TestCase;

import org.junit.Test;

import ru.caustic.lasertag.causticlasertagcontroller.RCSProtocol;

/**
 * Created by alexey on 19.09.15.
 */
public class RCSProtocolTest extends TestCase {

    private void testAnyParameterSerDeser(RCSProtocol.AnyParameterSerializer par, String originalValue, String otherValue, int bufferLength, int offset) {
        byte buffer[] = new byte[bufferLength];
        par.setValue(originalValue);
        par.serialize(buffer, offset);
        par.setValue(otherValue);
        par.deserialize(buffer, offset);
        Assert.assertEquals(originalValue, par.getValue());
    }

    @Test
    public void testOperationCodesManipulation()
    {
        int code = 2005;
        Assert.assertEquals(
                RCSProtocol.removeOperationTypeBits(RCSProtocol.makeOperationCodeType(code, RCSProtocol.OperationCodeType.SET_OBJECT)),
                code
        );
        Assert.assertEquals(
                RCSProtocol.dispatchOperationCodeType(RCSProtocol.makeOperationCodeType(code, RCSProtocol.OperationCodeType.SET_OBJECT)),
                RCSProtocol.OperationCodeType.SET_OBJECT
        );
        Assert.assertEquals(
                RCSProtocol.dispatchOperationCodeType(RCSProtocol.makeOperationCodeType(code, RCSProtocol.OperationCodeType.OBJECT_REQUEST)),
                RCSProtocol.OperationCodeType.OBJECT_REQUEST
        );
        Assert.assertEquals(
                RCSProtocol.dispatchOperationCodeType(RCSProtocol.makeOperationCodeType(code, RCSProtocol.OperationCodeType.CALL_REQUEST)),
                RCSProtocol.OperationCodeType.CALL_REQUEST
        );
        Assert.assertEquals(
                RCSProtocol.dispatchOperationCodeType(RCSProtocol.makeOperationCodeType(code, RCSProtocol.OperationCodeType.RESERVED)),
                RCSProtocol.OperationCodeType.RESERVED
        );
    }

    @Test
    public void testUint16SerializationDeserialization()
    {
        RCSProtocol.AnyParameterSerializer par = new RCSProtocol.UintParameterSerializer(null);
        testAnyParameterSerDeser(par, "1", "0", 10, 0);
        testAnyParameterSerDeser(par, "129", "0", 10, 1);
        testAnyParameterSerDeser(par, "160", "0", 10, 2);
        testAnyParameterSerDeser(par, "32768", "0", 10, 0);
        testAnyParameterSerDeser(par, "65535", "0", 10, 6);
        testAnyParameterSerDeser(par, "0", "1", 10, 0);
    }

    @Test
    public void testDeviceNameSerializationDeserialization() {
        RCSProtocol.AnyParameterSerializer par = new RCSProtocol.DevNameParameterSerializer(null);
        //RCSProtocol.Parameter par = new RCSProtocol.Parameter("Test", RCSProtocol.Parameter.TYPE_DEVICE_NAME, 123);
        testAnyParameterSerDeser(par, "Test name 1", "", 40, 20);
        testAnyParameterSerDeser(par, "The device ASCII na", "", 20, 0);

        // String cropping test
        byte buffer[] = new byte[20];
        par.setValue("123456789012345678901234567890");
        par.serialize(buffer, 0);
        par.setValue("");
        par.deserialize(buffer, 0);
        Assert.assertEquals("1234567890123456789", par.getValue());
    }

    @Test
    public void testMT2idSerializationDeserialization() {
        RCSProtocol.AnyParameterSerializer par = new RCSProtocol.MT2IdParameterSerializer(null);
        testAnyParameterSerDeser(par, "1", "0", 2, 0);
        testAnyParameterSerDeser(par, "80", "0", 2, 1);
        testAnyParameterSerDeser(par, "127", "0", 10, 5);
        testAnyParameterSerDeser(par, "0", "1", 2, 0);

    }

    @Test
    public void testIntSerializationDeserialization() {
        RCSProtocol.AnyParameterSerializer par = new RCSProtocol.IntParameterSerializer(null);
        testAnyParameterSerDeser(par, "1", "0", 2, 0);
        testAnyParameterSerDeser(par, "23767", "0", 10, 5);
        testAnyParameterSerDeser(par, "-1", "0", 4, 1);
        testAnyParameterSerDeser(par, "0", "1", 2, 0);
    }

    @Test
    public void testFloatSerializationDeserialization() {

        RCSProtocol.AnyParameterSerializer par = new RCSProtocol.FloatParameterSerializer(null);
        testAnyParameterSerDeser(par, Float.toString(Float.parseFloat("1.0")), "0.0", 4, 0);
        testAnyParameterSerDeser(par, Float.toString(Float.parseFloat("3.1415926")), "0.0", 4, 0);
        testAnyParameterSerDeser(par, Float.toString(Float.parseFloat("-243.6124123")), "0.0", 4, 0);
    }

    @Test
    public void testDevAddrSerializationDeserialization() {
        RCSProtocol.AnyParameterSerializer par = new RCSProtocol.DevAddrParameterSerializer(null);
        testAnyParameterSerDeser(par, "123.43.8", "0.0.0", 4, 0);
        testAnyParameterSerDeser(par, "255.240.1", "0.0.0", 4, 0);
        testAnyParameterSerDeser(par, "0.0.0", "1.2.3", 4, 0);

    }

    @Test
    public void testBoolSerializationDeserialization() {
        RCSProtocol.AnyParameterSerializer par = new RCSProtocol.BoolParameterSerializer(null);
        testAnyParameterSerDeser(par, "true", "false", 1, 0);
        testAnyParameterSerDeser(par, "false", "true", 4, 0);
    }

    @Test
    public void timeIntervalSerializationDeserialization() {
        RCSProtocol.AnyParameterSerializer par = new RCSProtocol.TimeIntervalParameterSerializer(null);
        testAnyParameterSerDeser(par, "2345", "0", 4, 0);
        testAnyParameterSerDeser(par, "1", "0", 4, 0);
        testAnyParameterSerDeser(par, "0", "24", 4, 0);
        testAnyParameterSerDeser(par, "4294967295", "1", 4, 0);
    }

    @Test
    public void testMT2IdSerializationDeserialization() {
        RCSProtocol.AnyParameterSerializer par = new RCSProtocol.MT2IdParameterSerializer(null);
        testAnyParameterSerDeser(par, "24", "0", 1, 0);
        testAnyParameterSerDeser(par, "127", "4", 4, 0);
    }

    @Test
    public void testUByteSerializationDeserialization() {
        RCSProtocol.AnyParameterSerializer par = new RCSProtocol.UByteParameterSerializer(null);
        testAnyParameterSerDeser(par, "24", "0", 1, 0);
        testAnyParameterSerDeser(par, "255", "4", 4, 0);
    }

    @Test
    public void testStreamReadWriteOneParameter() {
        RCSProtocol.ParametersDescriptionsContainer description = new RCSProtocol.ParametersDescriptionsContainer();
        RCSProtocol.ParametersContainer2 container = new RCSProtocol.ParametersContainer2();
        int paramId = 321;
        RCSProtocol.ParameterDescription testParam
            = new RCSProtocol.UintParameterDescription(description, paramId, "Test parameter description", 1, 200);
        description.addParameters(container);

        byte arr[] = new byte[20];
        String etalonValue = "48";

        container.get(paramId).setValue(etalonValue);
        int size = container.serializeSetObject(paramId, arr, 0, 20);
        Assert.assertTrue(size != 0);
        container.get(paramId).setValue("0");
        container.deserializeOneParamter(arr, 0, 20);
        Assert.assertEquals(etalonValue, container.get(paramId).getValue());
    }

    @Test
    public void testAllTypesSerDeserStream() {
        RCSProtocol.ParametersDescriptionsContainer description = new RCSProtocol.ParametersDescriptionsContainer();
        RCSProtocol.ParametersContainer2 container = new RCSProtocol.ParametersContainer2();
        RCSProtocol.ParameterDescription testParam1
                = new RCSProtocol.UintParameterDescription(description, 1, "Test uintparameter description", 1, 200);
        RCSProtocol.ParameterDescription testParam2
                = new RCSProtocol.IntParameterDescription(description, 2, "Test int parameter description", -150, 200);
        RCSProtocol.ParameterDescription testParam3
                = new RCSProtocol.FloatParameterDescription(description, 3, "Test float parameter description", true, 0.0f, 1.1f);
        RCSProtocol.ParameterDescription testParam4
                = new RCSProtocol.DevNameParameterDescription(description, 4, "Test device name parameter description", false);
        RCSProtocol.ParameterDescription testParam5
                = new RCSProtocol.BooleanParameterDescription(description, 5, "Test bool parameter description", true);
        RCSProtocol.ParameterDescription testParam6
                = new RCSProtocol.MT2IdParameterDescription(description, 6, "Test MT2 id parameter description", true);
        RCSProtocol.ParameterDescription testParam7
                = new RCSProtocol.DevAddrParameterDescription(description, 7, "Test dev address parameter description", true);
        RCSProtocol.ParameterDescription testParam8
                = new RCSProtocol.TimeIntervalParameterDescription(description, 8, "Test time interval parameter description", 0, 1_000_000);
        RCSProtocol.ParameterDescription testParam9
                = new RCSProtocol.UByteParameterDescription(description, 9, "Test unsigned byte parameter description", 0, 255);

        description.addParameters(container);

        int bufferSize = 80;
        byte arr[] = new byte[bufferSize];
        int ui = 2345;
        float f = Float.parseFloat(Float.toString(Float.parseFloat("-3.1415926")));
        String name = "Test name";
        int i = -2022;
        int id = 17;
        String addr = "101.255.0";
        long timeInt = 500_000;
        int ub = 211;

        container.get(1).setValue(Integer.toString(ui));
        container.get(2).setValue(Integer.toString(i));
        container.get(3).setValue(Float.toString(f));
        container.get(4).setValue(name);
        container.get(5).setValue("true");
        container.get(6).setValue(Integer.toString(id));
        container.get(7).setValue(addr);
        container.get(8).setValue(Long.toString(timeInt));
        container.get(9).setValue(Integer.toString(ub));

        int cursor = 0;
        cursor += container.serializeSetObject(1, arr, cursor, bufferSize);
        cursor += container.serializeSetObject(2, arr, cursor, bufferSize);
        cursor += container.serializeSetObject(3, arr, cursor, bufferSize);
        cursor += container.serializeSetObject(4, arr, cursor, bufferSize);
        cursor += container.serializeSetObject(5, arr, cursor, bufferSize);
        cursor += container.serializeSetObject(6, arr, cursor, bufferSize);
        cursor += container.serializeSetObject(7, arr, cursor, bufferSize);
        cursor += container.serializeSetObject(8, arr, cursor, bufferSize);
        cursor += container.serializeSetObject(9, arr, cursor, bufferSize);
        container.get(1).setValue("0");
        container.get(2).setValue("0");
        container.get(3).setValue("No");
        container.get(4).setValue("0");
        container.get(5).setValue("false");
        container.get(6).setValue("45");
        container.get(7).setValue("12.34.56");
        container.get(8).setValue("10000");
        container.get(9).setValue("10");

        // Testing now
        container.deserializeStream(arr, 0, cursor);
        Assert.assertEquals(Integer.toString(ui), container.get(1).getValue());
        Assert.assertEquals(Integer.toString(i), container.get(2).getValue());
        Assert.assertEquals(Float.toString(f), container.get(3).getValue());
        Assert.assertEquals(name, container.get(4).getValue());
        Assert.assertEquals("true", container.get(5).getValue());
        Assert.assertEquals(Integer.toString(id), container.get(6).getValue());
        Assert.assertEquals(addr, container.get(7).getValue());
        Assert.assertEquals(timeInt, Long.parseLong(container.get(8).getValue()));
        Assert.assertEquals(ub, Integer.parseInt(container.get(9).getValue()));
    }
}