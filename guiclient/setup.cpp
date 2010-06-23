/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2010 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */
#include <QBuffer>
#include <QUiLoader>
#include <QMessageBox>
#include <QPushButton>
#include <QScriptEngine>
#include <QScriptEngineDebugger>

#include "getscreen.h"
#include "scripttoolbox.h"
#include "setup.h"
#include "xt.h"
#include "xtreewidget.h"
#include "xwidget.h"

setup::setup(QWidget* parent, const char* name, bool modal, Qt::WFlags fl)
    : XDialog(parent, name, modal, fl)
{
  setupUi(this);

  _modules->addItem(tr("All"), Xt::AllModules);
  _modules->addItem(tr("Accounting"), Xt::AccountingModule);
  _modules->addItem(tr("Sales"), Xt::SalesModule);
  _modules->addItem(tr("CRM"), Xt::CRMModule);
  _modules->addItem(tr("Manufacture"), Xt::ManufactureModule);
  _modules->addItem(tr("Purchase"), Xt::PurchaseModule);
  _modules->addItem(tr("Schedule"), Xt::ScheduleModule);
  _modules->addItem(tr("Inventory"), Xt::InventoryModule);
  _modules->addItem(tr("Products"), Xt::ProductsModule);
  _modules->addItem(tr("System"), Xt::SystemModule);

  QPushButton* apply = _buttonBox->button(QDialogButtonBox::Apply);
  connect(apply, SIGNAL(clicked()), this, SLOT(apply()));
  connect(_buttonBox, SIGNAL(accepted()), this, SLOT(save()));
  connect(_modules, SIGNAL(currentIndexChanged(int)), this, SLOT(populate()));
  connect(_tree, SIGNAL(currentItemChanged(XTreeWidgetItem*,XTreeWidgetItem*)), this, SLOT(setCurrentIndex(XTreeWidgetItem*)));

  _tree->addColumn(QString(), -1, Qt::AlignLeft, true);
  _tree->setHeaderHidden(true);

  // Configure
  insert(tr("Accounting"), "configureGL", Configure, Xt::AccountingModule, mode("ConfigureGL"), 0, "sSave()");
  insert(tr("Credit Card"), "configureCC", Configure, Xt::SystemModule, mode("ConfigureCC"), 0, "sSave()");
  insert(tr("CRM"), "configureCRM", Configure, Xt::CRMModule, mode("ConfigureCRM"), 0, "sSave()" );
  insert(tr("Database"), "databaseInformation", Configure, Xt::SystemModule, mode("ConfigDatabaseInfo"), 0, "sSave()");
  insert(tr("Encryption"), "configureEncryption", Configure, Xt::SystemModule, mode("ConfigureEncryption"), 0, "sSave()");
  insert(tr("Inventory"), "configureIM", Configure, Xt::InventoryModule, mode("ConfigureIM"), 0, "sSave()" );
  insert(tr("Import/Export"), "configureIE", Configure, Xt::SystemModule, mode("ConfigureImportExport"), 0, "sSave()");
  insert(tr("Sales"), "configureSO", Configure, Xt::SalesModule, mode("ConfigureSO"), 0, "sSave()" );
  insert(tr("Manufacture"), "configureWO", Configure, Xt::ManufactureModule, mode("ConfigureWO"), 0, "sSave()");
  insert(tr("Products"), "configurePD", Configure, Xt::ProductsModule, mode("ConfigurePD"), 0, "sSave()" );
  insert(tr("Purchase"), "configurePO", Configure, Xt::PurchaseModule, mode("ConfigurePO"), 0, "sSave()" );
  insert(tr("Schedule"), "configureMS", Configure, Xt::ScheduleModule, mode("ConfigureMS"), 0, "sSave()" );

  // Account Mappings
  int modeVal;
  modeVal = mode("MaintainCostCategories", "ViewCostCategories");
  insert(tr("Cost Categories"), "costCategories", AccountMapping,  Xt::AccountingModule | Xt::InventoryModule, modeVal, modeVal);

  modeVal = mode("MaintainExpenseCategories", "ViewExpenseCategories");
  insert(tr("Expense Categories"), "expenseCategories", AccountMapping, Xt::AccountingModule |Xt::InventoryModule | Xt::PurchaseModule, modeVal, modeVal);

  modeVal = mode("MaintainVendorAccounts", "ViewVendorAccounts");
  insert(tr("Payables Assignments"), "apAccountAssignments", AccountMapping, Xt::AccountingModule | Xt::PurchaseModule, modeVal, modeVal);

  modeVal = mode("MaintainSalesAccount", "ViewSalesAccount");
  insert(tr("Receivables Assignments"), "arAccountAssignments", AccountMapping, Xt::AccountingModule | Xt::SalesModule, modeVal, modeVal);
  insert(tr("Sales Assignments"), "salesAccounts", AccountMapping, Xt::AccountingModule | Xt::SalesModule, modeVal, modeVal);

  modeVal = mode("MaintainSalesCategories", "ViewSalesCategories");
  insert(tr("Sales Categories"),  "salesCategories", AccountMapping, Xt::AccountingModule | Xt::SalesModule, modeVal, modeVal);

  // Master Information
  modeVal = mode("MaintainBankAccounts");
  insert(tr("Bank Accounts"), "bankAccounts", MasterInformation, Xt::AccountingModule, modeVal, modeVal);

  modeVal = mode("MaintainAdjustmentTypes", "ViewAdjustmentTypes");
  insert(tr("Bank Adjustment Types"), "bankAdjustmentTypes", MasterInformation, Xt::AccountingModule, modeVal, modeVal);

  modeVal = mode("MaintainCalendars");
  insert(tr("Calendars"), "calendars", MasterInformation,  Xt::SystemModule, modeVal, modeVal);

  modeVal = mode("MaintainCharacteristics", "ViewCharacteristics");
  insert(tr("Characteristics"), "characteristics", MasterInformation,
         Xt::ProductsModule | Xt::InventoryModule | Xt::CRMModule |
         Xt::SalesModule | Xt::AccountingModule, modeVal, modeVal);

  modeVal = mode("MaintainCheckFormats", "ViewCheckFormats");
  insert(tr("Check Formats"), "checkFormats", MasterInformation, Xt::AccountingModule, modeVal, modeVal);

  modeVal = mode("MaintainClassCodes", "ViewClassCodes");
  insert( tr("Class Codes"), "classCodes", MasterInformation, Xt::ProductsModule, modeVal, modeVal);

  modeVal = mode("MaintainCommentTypes");
  insert(tr("Comment Types"), "commentTypes", MasterInformation, Xt::SystemModule, modeVal, modeVal);

  modeVal = mode("MaintainCountries");
  insert(tr("Countries"), "countries",  MasterInformation, Xt::SystemModule, modeVal, modeVal);

  modeVal = mode("CreateNewCurrency");
  insert(tr("Currencies"), "currencies", MasterInformation,  Xt::SystemModule, modeVal, modeVal);

  modeVal = mode("MaintainCustomerMasters");
  insert(tr("Customer Form Assignments"), "customerFormAssignments", MasterInformation,  Xt::SalesModule, modeVal, modeVal);

  modeVal = mode("MaintainCustomerTypes", "ViewCustomerTypes");
  insert(tr("Customer Types"), "customerTypes",  MasterInformation, Xt::SalesModule | Xt::AccountingModule, modeVal, modeVal);

  modeVal = mode("ViewDepartments", "MaintainDepartments");
  insert(tr("Departments"), "departments", MasterInformation, Xt::SystemModule, modeVal, modeVal);

  modeVal = mode("MaintainCurrencyRates", "ViewCurrencyRates");
  insert(tr("Exchange Rates"), "currencyConversions", MasterInformation, Xt::SystemModule, modeVal, modeVal);

  modeVal = mode("MaintainForms");
  insert( tr("Forms"),  "forms", MasterInformation, Xt::SystemModule, modeVal, modeVal);

  modeVal = mode("MaintainFreightClasses", "ViewFreightClasses");
  insert(tr("Freight Classes"), "freightClasses", MasterInformation, Xt::ProductsModule, modeVal, modeVal);

  modeVal = mode("MaintainImages");
  insert(tr("Images"), "images", MasterInformation, Xt::SystemModule, modeVal, modeVal);

  modeVal = mode("MaintainIncidentCategories");
  insert(tr("Incident Categories"), "incidentCategories", MasterInformation, Xt::CRMModule, modeVal, modeVal);

  modeVal = mode("MaintainIncidentPriorities");
  insert(tr("Incident Priorities"), "incidentPriorities", MasterInformation, Xt::CRMModule, modeVal, modeVal);

  modeVal = mode("MaintainIncidentResolutions");
  insert(tr("Incident Resolutions"), "incidentResolutions", MasterInformation, Xt::CRMModule, modeVal, modeVal);

  modeVal = mode("MaintainIncidentSeverities");
  insert(tr("Incident Severities"), "incidentSeverities", MasterInformation, Xt::CRMModule, modeVal, modeVal);

  modeVal = mode("MaintainForms");
  insert( tr("Label Forms"), "labelForms", MasterInformation, Xt::SystemModule, modeVal, modeVal);

  modeVal = mode("MaintainLocales");
  insert(tr("Locales"), "locales", MasterInformation, Xt::SystemModule, modeVal, modeVal);

  if ( _metrics->boolean("LotSerialControl"))
  {
    modeVal = mode("MaintainLotSerialSequences", "ViewLotSerialSequences");
    insert(tr("Lot/Serial Sequences"), "lotSerialSequences", MasterInformation, Xt::ProductsModule, modeVal, modeVal);
  }

  modeVal = mode("MaintainOpportunitySources");
  insert( tr("Opportunity Sources"), "opportunitySources", MasterInformation, Xt::CRMModule, modeVal, modeVal);

  modeVal = mode("MaintainOpportunityStages");
  insert(tr("Opportunity Stages"), "opportunityStages", MasterInformation, Xt::CRMModule, modeVal, modeVal);

  modeVal = mode("MaintainOpportunityTypes");
  insert(tr("Opportunity Types"), "opportunityTypes", MasterInformation, modeVal, modeVal);

  modeVal = mode("MaintainPlannerCodes", "ViewPlannerCodes");
  insert(tr("Planner Codes"), "plannerCodes", MasterInformation, Xt::InventoryModule | Xt::ScheduleModule | Xt::PurchaseModule, modeVal, modeVal);

  modeVal = mode("MaintainProductCategories", "ViewProductCategories");
  insert(tr("Product Categories"), "productCategories", MasterInformation, Xt::ProductsModule, modeVal, modeVal);

  modeVal = mode("MaintainReasonCodes");
  insert(tr("Reason Codes"), "reasonCodes", MasterInformation, Xt::InventoryModule | Xt::AccountingModule, modeVal, modeVal);

  modeVal = mode("MaintainRejectCodes", "ViewRejectCodes");
  insert(tr("Reject Codes"), "rejectCodes", MasterInformation, Xt::PurchaseModule, modeVal, modeVal);

  modeVal = mode("MaintainSalesReps", "ViewSalesReps");
  insert(tr("Sales Reps"), "salesReps",  MasterInformation, Xt::SalesModule, modeVal, modeVal);

  modeVal = mode("MaintainStates");
  insert(tr("States and Provinces"), "states", MasterInformation, Xt::SystemModule, modeVal, modeVal);

  modeVal = mode("MaintainShippingChargeTypes", "ViewShippingChargeTypes");
  insert(tr("Shipping Charge Types"), "shippingChargeTypes", MasterInformation, Xt::SalesModule, modeVal, modeVal);

  modeVal = mode("MaintainShippingForms", "ViewShippingForms");
  insert( tr("Shipping Forms"), "shippingForms", MasterInformation, Xt::SalesModule, modeVal, modeVal);

  modeVal = mode("MaintainShippingZones", "ViewShippingZones");
  insert(tr("Shipping Zones"), "shippingZones", MasterInformation, Xt::SalesModule, modeVal, modeVal);

  modeVal = mode("MaintainShipVias", "ViewShipVias");
  insert(tr("Ship Vias"), "shipVias", MasterInformation, Xt::SalesModule, modeVal, modeVal);

  modeVal = mode("MaintainSiteTypes", "ViewSiteTypes");
  insert(tr("Site Types"), "siteTypes", MasterInformation, Xt::InventoryModule, modeVal, modeVal);

  modeVal = mode("MaintainTaxCodes", "ViewTaxCodes");
  insert(tr("Tax Codes"), "taxCodes", MasterInformation, Xt::SalesModule, modeVal, modeVal);

  modeVal = mode("MaintainTerms", "ViewTerms");
  insert(tr("Terms"), "termses", MasterInformation, Xt::PurchaseModule | Xt::SalesModule | Xt::AccountingModule, modeVal, modeVal);

  modeVal = mode("MaintainTitles", "ViewTitles");
  insert(tr("Titles"), "honorifics", MasterInformation, Xt::CRMModule, modeVal, modeVal);

  modeVal = mode("MaintainUOMs", "ViewUOMs");
  insert(tr("Units of Measure"), "uoms", MasterInformation, Xt::ProductsModule, modeVal, modeVal);

  modeVal = mode("MaintainVendorTypes", "ViewVendorTypes");
  insert(tr("Vendor Types"),  "vendorTypes", MasterInformation, Xt::PurchaseModule | Xt::AccountingModule, modeVal, modeVal);

}

setup::~setup()
{
}

enum SetResponse setup::set(const ParameterList &pParams)
{
  XDialog::set(pParams);
  QVariant param;
  bool     valid;

  param = pParams.value("module", &valid);
  if (valid)
  {
    for (int i = 0; i < _modules->count(); i++)
    {
      if (_modules->itemData(i) == param.toInt())
        _modules->setCurrentIndex(i);
    }
  }
  else
    populate();

  return NoError;
}

/*!
  Add setup \a widget with XTreewidgetItem \a parent to the list of setup items and the stacked widget with
  a\ title.  The item on the list will be enabled according to the \a enabled value.  The value of \a mode will
  determined whether parameters passed open the widget in "edit" or "view" mode.  A save function on the
  widget triggered by the Apply and Save buttons can be specified by \a saveMethod.
*/
void setup::insert(const QString &title, const QString &uiName, int type, int modules, bool enabled, int mode, const QString &saveMethod)
{
   ItemProps ip;

   ip.uiName = uiName;
   ip.type = type;
   ip.modules = modules;
   ip.enabled = enabled;
   ip.mode = mode;
   ip.saveMethod = saveMethod;

   _itemMap.insert(title, ip);
}

/*!
  Saves the current metric settings and repopulates the window.
*/
void setup::apply()
{
  int id = _tree->id();
  save(false);
  populate(id == -1);
  if (id > -1)
  {
    _tree->setId(id);
    setCurrentIndex(_tree->currentItem());
  }
}

void setup::languageChange()
{
  retranslateUi(this);
}

/*!
  Returns the mode value based on the privileges granted by checking \a editPriv and
  \a viewPriv.  If the user has edit privileges cEdit (2) will be returned, if only view
  privileges then cView (3) will be returned, otherwise 0;
  */
int setup::mode(const QString &editPriv, const QString &viewPriv)
{
  if (_privileges->check(editPriv))
    return cEdit;
  else if (_privileges->check(viewPriv))
    return cView;

  return 0;
}

/*!
  Populates the list of setup widgets filtered by selectd module.  Selects the first item
  when \a first is true.
  */
void setup::populate(bool first)
{
  _tree->clear();
  _idxmap.clear();
  while (_stack->count())
  {
    QWidget* w = _stack->widget(0);
    _stack->removeWidget(w);
    w = 0;
  }

  XTreeWidgetItem* _configItem = new XTreeWidgetItem(_tree, 0, -1, tr("Configure"));
  XTreeWidgetItem* _mapItem = new XTreeWidgetItem(_tree, 0, -1, tr("Accounting Mappings"));
  XTreeWidgetItem* _masterItem = new XTreeWidgetItem(_tree, 0, -1, tr("Master Information"));
  QBrush disabled(Qt::gray);
  XTreeWidgetItem* parent = 0;
  ItemProps ip;
  int id = 0;

  QMapIterator<QString, ItemProps> i(_itemMap);
  while (i.hasNext())
  {
    id++;
    i.next();
    ip = i.value();

    if (_modules->currentIndex() == 0 ||
        _modules->itemData(_modules->currentIndex()).toInt() & ip.modules)
    {
      if (ip.type == Configure)
        parent = _configItem;
      else if (ip.type == AccountMapping)
        parent = _mapItem;
      else
        parent = _masterItem;

      // Set the item on the list
      XTreeWidgetItem* item  = new XTreeWidgetItem(parent, id);
      item->setData(0, Qt::DisplayRole, QVariant(i.key()));
      item->setData(0, Xt::RawRole, QVariant(ip.uiName));

      if (!ip.enabled)
      {
        item->setFlags(Qt::NoItemFlags);
        item->setForeground(0,disabled);
      }
      parent->addChild(item);

      // Store the save methad name
      if (!ip.saveMethod.isEmpty())
        _methodMap.insert(ip.uiName, ip.saveMethod);
    }
  }

  if (!_configItem->childCount())
    _tree->takeTopLevelItem(_tree->indexOfTopLevelItem(_configItem));

  if (!_mapItem->childCount())
    _tree->takeTopLevelItem(_tree->indexOfTopLevelItem(_mapItem));

  if (!_masterItem->childCount())
    _tree->takeTopLevelItem(_tree->indexOfTopLevelItem(_masterItem));

  _tree->expandAll();
  if (_tree->topLevelItemCount() && first)
    setCurrentIndex(_tree->topLevelItem(0));

}

/*! Emits the \a saving() signal which triggers any widgets to save that have a mapped \a savedMethod()
  specified by \sa insert().  Also reloads metrics, privileges, preferences, and the menubar in the
  main application.  The screen will close if \a close is true.
  */
void setup::save(bool close)
{
  emit saving();
  _metrics->load();
  _privileges->load();
  _preferences->load();
  omfgThis->initMenuBar();

  if (close)
    accept();
}

void setup::setCurrentIndex(XTreeWidgetItem* item)
{
  QString uiName = item->data(0, Xt::RawRole ).toString();
  QString label = "<span style=\" font-size:14pt; font-weight:600;\">%1</span></p></body></html>";
  qDebug("setting class name " + uiName );

  if (_idxmap.contains(uiName))
  {
    _stack->setCurrentIndex(_idxmap.value(uiName));
    _stackLit->setText(label.arg(item->text(0)));
    return;
  }
  else if (!item->isDisabled())
  {
    // First look for a class...
    QWidget *w = xtGetScreen(uiName, this);

    if (w)
    {
      //Connect save slot if applicable
      if (_methodMap.contains(uiName))
      {
        QString method = _methodMap.value(uiName);
        connect(this, SIGNAL(saving()), w, QString("1%1").arg(method).toUtf8().data());
      }
    }
    else
    {
      // No class, so look for an extension
      XSqlQuery screenq;
      screenq.prepare("SELECT * "
                      "  FROM uiform "
                      " WHERE((uiform_name=:uiform_name)"
                      "   AND (uiform_enabled))"
                      " ORDER BY uiform_order DESC"
                      " LIMIT 1;");
      screenq.bindValue(":uiform_name", uiName);
      screenq.exec();
      if (screenq.first())
      {     
        QUiLoader loader;
        QByteArray ba = screenq.value("uiform_source").toByteArray();
        QBuffer uiFile(&ba);
        if (!uiFile.open(QIODevice::ReadOnly))
          QMessageBox::critical(0, tr("Could not load UI"),
                                tr("<p>There was an error loading the UI Form "
                                   "from the database."));
        w = loader.load(&uiFile);
        w->setObjectName(uiName);
        uiFile.close();

        // Load scripts if applicable
        XSqlQuery scriptq;
        scriptq.prepare("SELECT script_source, script_order"
                        "  FROM script"
                        " WHERE((script_name=:script_name)"
                        "   AND (script_enabled))"
                        " ORDER BY script_order;");
        scriptq.bindValue(":script_name", uiName);
        scriptq.exec();

        QScriptEngine* engine = new QScriptEngine();
        if (_preferences->boolean("EnableScriptDebug"))
        {
          QScriptEngineDebugger* debugger = new QScriptEngineDebugger(this);
          debugger->attachTo(engine);
        }
        omfgThis->loadScriptGlobals(engine);
        QScriptValue mywindow = engine->newQObject(w);
        engine->globalObject().setProperty("mywindow", mywindow);

        while(scriptq.next())
        {
          QString script = scriptHandleIncludes(scriptq.value("script_source").toString());
          QScriptValue result = engine->evaluate(script, uiName);
          if (engine->hasUncaughtException())
          {
            int line = engine->uncaughtExceptionLineNumber();
            qDebug() << "uncaught exception at line" << line << ":" << result.toString();
          }

          //Connect save slot if applicable
          if (_methodMap.contains(uiName))
          {
            QString method = QString(_methodMap.value(uiName)).remove("(").remove(")");

            if(engine->globalObject().property(method).isFunction())
            {
              QScriptValue slot = engine->globalObject().property(method);
              qScriptConnect(this, SIGNAL(saving()), result, slot );
            }
          }
        }
      }
    }

    if (w)
    {
      // Hide buttons out of context here
      QWidget* close = w->findChild<QWidget*>("_close");
      if (close)
        close->hide();
      QWidget* buttons = w->findChild<QDialogButtonBox*>();
      if (buttons)
        buttons->hide();

      //Set mode if applicable
      int mode = _itemMap.value(item->text(0)).mode;
      if (mode && w->inherits("XDialog"))
      {
        XWidget* x = dynamic_cast<XWidget*>(w);
        ParameterList params;
        if (mode == cEdit)
          params.append("mode", "edit");
        else if (mode == cView)
          params.append("mode", "view");
        x->set(params);
      }

      int idx = _stack->count();
      _idxmap.insert(uiName,idx);
      _stack->addWidget(w);
      _stack->setCurrentIndex(idx);

      _stackLit->setText(label.arg(item->text(0)));
      return;
    }
  }
  // Nothing here so try the next one
  XTreeWidgetItem* next = dynamic_cast<XTreeWidgetItem*>(_tree->itemBelow(item));
  if (next)
    setCurrentIndex(next);
}

