import { Injectable } from '@angular/core';
import { AuthHttp } from 'angular2-jwt';
import { Observable } from 'rxjs/Observable';

import { DataService, Data } from './data.service';

export class Agent {

    public id: Number;
    public description: String;
    public uploadInterval: Number;
    public snapshotInterval: Number;
    public widthResolution: Number;
    public heightResolution: Number;
    public initialContact: Boolean;
    public datas: Data[];
    public loading: boolean;

    constructor(json: any, private dataService: DataService) {
        this.id = json.id;
        this.description = json.description;
        this.uploadInterval = json.uploadInterval;
        this.snapshotInterval = json.snapshotInterval;
        this.widthResolution = json.widthResolution;
        this.heightResolution = json.heightResolution;
        this.initialContact = json.initialContact;

        this.loading = true;
        this.dataService.all(this.id)
            .subscribe(result => {
                this.datas = result.sort((a, b) => a.integrationDate.valueOf() - b.integrationDate.valueOf());
                this.loading = false;
            });
    }

    getLastImagePath(): String {
        let images = this.datas.filter(d => d.type == Data.DATA_IMAGE);
        if (images.length > 0) {
            return images[images.length - 1].value;
        }
        return "";
    }

    getLastContact(): String {
        if (this.datas.length > 0) {
            let mostRecent = this.datas[this.datas.length - 1];
            return mostRecent.getIntegrationTime();
        }
        return "jamais";
    }
}

@Injectable()
export class AgentService {
    constructor(private authHttp: AuthHttp, private dataService: DataService) { }

    all(): Observable<Agent[]> {
        return this.authHttp.get("/api/agent")
            .map(res => res.json().map((js: any) => new Agent(js, this.dataService)));
    }

    find(id: Number): Observable<Agent> {
        return this.authHttp.get("/api/agent/" + id)
            .map(res => new Agent(res.json(), this.dataService));
    }

    update(agent: Agent): Observable<Number> {
        return this.authHttp.put("/api/agent/" + agent.id, agent)
            .map(res => res.json().state);
    }
}