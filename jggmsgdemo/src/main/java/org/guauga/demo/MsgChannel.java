package org.guauga.demo;

import jggmsg.ChannelListener;
import jggmsg.ChannelNode;

public class MsgChannel extends ChannelListener {

    ChannelNode node = null;
    private int lastConnectId;
    public void init(){
        node = ChannelNode.Create(this);
    }

    public void connect(String host, short port){
        node.Connect(host, port);
    }

    public void start(int serviceId, short port){
        node.Start(serviceId, port);
    }

    public void sendTestMsg(byte [] msg){
        node.SendToConnect(lastConnectId, msg);
    }

    @Override
    public void OnPassiveConnect(int serviceId, int connectId) {
        lastConnectId = connectId;
        System.out.println("OnPassiveConnect");
    }

    @Override
    public void OnPassiveDisConnect(int serviceId, int connectId) {

    }

    @Override
    public void OnPositiveConnect(int serviceId, int connectId) {

    }

    @Override
    public void OnPositiveDisConnect(int serviceId, int connectId) {

    }

    @Override
    public void OnReceiveMsg(int serviceId, int connectId, byte[] bytes) {

    }
}
